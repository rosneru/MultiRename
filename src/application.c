#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <libraries/locale.h>
#include <utility/hooks.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include <clib/compiler-specific.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/dos_protos.h>
  #include <clib/intuition_protos.h>
  #include <clib/button_protos.h>
  #include <clib/chooser_protos.h>
  #include <clib/integer_protos.h>
  #include <clib/label_protos.h>
  #include <clib/layout_protos.h>
  #include <clib/listbrowser_protos.h>
  #include <clib/locale_protos.h>
  #include <clib/string_protos.h>
  #include <clib/window_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/intuition.h>
  #include <proto/button.h>
  #include <proto/chooser.h>
  #include <proto/integer.h>
  #include <proto/label.h>
  #include <proto/layout.h>
  #include <proto/listbrowser.h>
  #include <proto/locale.h>
  #include <proto/string.h>
  #include <proto/window.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_list.h"
#include "notifications.h"
#include "range_select_window.h"
#include "rename_algorithm.h"
#include "requester.h"
#include "ui_tools.h"
#include "application.h"

/// Private function declarations

/**
 * Re-attaches the list of FileNodes to the list browser.
 * Set the current working path if necessary.
 * Updates window title with the current working path.
 * 
 * NOTE: List must be detached off ListBrowser before this call!
 */
void applyNewFiles(Application* pApp);

/**
 * When range selection window was closed with Ok, the resulting mask is
 * inserted in current cursor position.
 */
void applySelectedRange(Application* pApp);

/**
 * Set the current working path as application window title.
 */
void updateApplicationWindowTitle(Application* pApp);

/**
 * Calculates the new names in the processing list / ListBrowser.
 */
BOOL updateNewNames(Application* pApp);

/**
 * Informs the user about error / skip notifications, if there are some.
 * With the option to display the details.
 */
void notifyUserAboutSkippedFiles(Application* pApp);

/**
 * Fill given pDest by inserting a command like [N12-16] at insert
 * position into given pSrcStr. pSrcStr is not changed, only part wise
 * copied into pDest.
 *
 * The inserted command is created of the insertCmd character, which can
 * be every char, but only 'N' and 'E' are interpret by the caller for
 * now, and the range insertFrom and insertTo.
 */
int insertPartIntoString(STRPTR pDest,
                         STRPTR pSrcStr,
                         UBYTE insertPos,
                         char insertCmd,
                         UBYTE insertFrom,
                         UBYTE insertTo);

/**
 * The application event loop.
 */
void intuiEventLoop(Application* pApp);

/**
 * Create layout for main window.
 */
Object* createLayout(void);

/// Private variables

struct ColumnInfo *m_pColumnInfo = NULL;

static UBYTE *m_ppCounterPlaces[] = { "1", "2", "3", "4", "5",
                                      "6", "7", "8", "9", "10", NULL };

enum gadids
{
    GID_STR_NAME = 1
  , GID_BTN_NAME
  , GID_BTN_NAME_PART
  , GID_BTN_NAME_DATE
  , GID_BTN_NAME_TIME
  , GID_BTN_NAME_COUNTER
  , GID_STR_EXTENSION
  , GID_BTN_EXTENSION
  , GID_BTN_EXTENSION_PART
  , GID_BTN_EXTENSION_COUNTER
  , GID_INT_COUNTER_START
  , GID_INT_COUNTER_STEP
  , GID_CHO_COUNTER_PLACES
  , GID_LBR_PROCESSING_LIST
  , GID_BTN_START
  , MAXGADGETS
};

static Object* m_ppGadgets[MAXGADGETS];
struct Hook m_CompareHook;
struct Hook m_AppHook;



/// Hook implementations

void __ASM__ __SAVE_DS__ AppMsgFunc(__REG__(a0, struct Hook *pHook),
                                    __REG__(a2, Object *pWindow),
                                    __REG__(a1, struct AppMessage *pMsg))
{
  ULONG i;
  STRPTR pFileName;
  struct WBArg *pWbArg = pMsg->am_ArgList;
  Application* pApp = (Application*)pHook->h_Data;

  // Detach list from ListBrowser. Must be done before changing the list.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, ~0,
                  TAG_DONE);

  // Iterate the files of the args and append them as FileNode if possible
  for(i = 0; i < pMsg->am_NumArgs; i++)
  {
    pFileName = pWbArg[i].wa_Name;
    if(NameFromLock(pWbArg[i].wa_Lock,
                    pApp->pParsedArgs->pScratchPathBuf,
                    MAXPATHLEN))
    {
      // Now scratch buf contains the name of the directory of the file
      // So next the fileName is appended to the buf
      AddPart(pApp->pParsedArgs->pScratchPathBuf, pFileName, MAXPATHLEN);

      appendFileNode(pApp->pFiles,
                     pApp->pParsedArgs->pScratchPathBuf,
                     pApp->pLocale,
                     pApp->pNotifications);
    }
    else
    {
      if(IoErr() == ERROR_LINE_TOO_LONG)
      {
        // For the error notification only the file name not the
        // relative path is needed.
        addNotification(pApp->pNotifications,
                        NNT_SKIPPED_PATH_TOO_LONG,
                        pFileName);
      }
    }
  }

  applyNewFiles(pApp);
}

/// Public function implementations

Application* createApplication(int argc, char **argv)
{
  Object* pMainLayout;
  Application* pApp;

  if((pApp = AllocVec(sizeof(Application), MEMF_CLEAR)))
  {
    if((pApp->pLocale = OpenLocale(NULL)))
    {
      if((pApp->pAppWindowPort = CreateMsgPort()))
      {
        if((pApp->pNotifications = createNotificationList()))
        {
          if((pApp->pFiles = createFileList()))
          {
            if((pApp->pParsedArgs = createParsedArgs(argc,
                                                    argv,
                                                    pApp->pFiles,
                                                    pApp->pLocale,
                                                    pApp->pNotifications)))
            {
              if((pMainLayout = createLayout()))
              {
                if((pApp->pWinObject = NewObject(WINDOW_GetClass(), NULL,
                                                WINDOW_Position, WPOS_CENTERSCREEN,
                                                WA_Activate, TRUE,
                                                WA_Title, "MultiRename",
                                                WA_CloseGadget, TRUE,
                                                WA_DepthGadget, TRUE,
                                                WA_DragBar, TRUE,
                                                WA_SizeGadget, TRUE,
                                                WA_InnerWidth, 600,
                                                WA_InnerHeight, 400,
                                                WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_GADGETUP,
                                                WINDOW_Layout, pMainLayout,
                                                WINDOW_AppPort, pApp->pAppWindowPort,
                                                WINDOW_AppWindow, TRUE,
                                                WINDOW_AppMsgHook, &m_AppHook,
                                                TAG_DONE)))
                {
                  if((pApp->pRangeSelectWindow = createRangeSelectWindow()))
                  {
                    return pApp;
                  }
                  else
                  {
                    PutStr("Failed to create the range select window.\n");
                    disposeApplication(pApp);
                  }
                }
                else
                {
                  PutStr("Failed to create the application main window.\n");
                  DisposeObject(pMainLayout);
                  disposeApplication(pApp);
                }
              }
              else
              {
                PutStr("Failed to create layout.\n");
                disposeApplication(pApp);
              }
            }
            else
            {
              PutStr("Failed to parse the arguments.\n");
              disposeApplication(pApp);
            }
          }
          else
          {
            PutStr("Failed to create the files list.\n");
            disposeApplication(pApp);
          }
        }
        else
        {
          PutStr("Failed to create the notifications object.\n");
          disposeApplication(pApp);
        }

      }
      else
      {
          PutStr("Failed to create the message port for window drag'n drop.\n");
          disposeApplication(pApp);
      }
    }
    else
    {
      PutStr("Failed to open the default Locale.\n");
      disposeApplication(pApp);
    }
  }
  else
  {
    PutStr("Failed to allocate memory for application instance data.\n");
  }

  return NULL;
}

void disposeApplication(Application* pApp)
{
  if(!pApp)
  {
    return;
  }

  if(pApp->pRangeSelectWindow)
  {
    freeRangeSelectWindow(pApp->pRangeSelectWindow);
  }

  if(pApp->pWinObject)
  {
    DisposeObject(pApp->pWinObject);
  }

  if(m_pColumnInfo)
  {
    FreeLBColumnInfo(m_pColumnInfo);
  }

  if(pApp->pParsedArgs)
  {
    freeParsedArgs(pApp->pParsedArgs);
  }

  if(pApp->pFiles)
  {
    freeFileList(pApp->pFiles);
  }

  if(pApp->pNotifications)
  {
    freeNotificationList(pApp->pNotifications);
  }

  if(pApp->pAppWindowPort)
  {
    DeleteMsgPort(pApp->pAppWindowPort);
  }

  if(pApp->pLocale)
  {
    CloseLocale(pApp->pLocale);
  }

  FreeVec(pApp);
}

BOOL runApplication(Application* pApp)
{
  if(!pApp)
  {
    return FALSE;
  }

  m_AppHook.h_Entry = (ULONG (* )())AppMsgFunc;
  m_AppHook.h_SubEntry = NULL;
  m_AppHook.h_Data = pApp;


  if((pApp->pIntuiWindow =
    (struct Window*)DoMethod(pApp->pWinObject, WM_OPEN, NULL)))
  {
    applyNewFiles(pApp);

    intuiEventLoop(pApp);

    // TODO: ClearMenuStrip()? before this..once a menu exists
    DoMethod(pApp->pWinObject, WM_CLOSE);

    return TRUE;
  }
  else
  {
    PutStr("Failed to open window.\n");
  }

  return FALSE;
}


/// Private function implementations

void updateApplicationWindowTitle(Application* pApp)
{
  if(strlen(pApp->FilesPath) > 0)
  {
    strcpy(pApp->WindowTitle, "MultiRename in [");
    strcat(pApp->WindowTitle, pApp->FilesPath);
    strcat(pApp->WindowTitle, "]");
    SetWindowTitles(pApp->pIntuiWindow, pApp->WindowTitle, (UBYTE *)~0);
  }
}

void notifyUserAboutSkippedFiles(Application* pApp)
{
  if(containsSkippedNotifications(pApp->pNotifications))
  {
    if(!showEasyRequest(pApp->pIntuiWindow,
                        "Continue|Show errors",
                        "Failed to add some of the input files"))
    {
      printNotifications(pApp->pNotifications);
      clearNotificationsExcept(pApp->pNotifications,
                                NNT_SELECTED_PATH_INFO);
    }
  }
}

void applyNewFiles(Application* pApp)
{
  STRPTR pFirstPath;

  // Does list contain at least one file?
  if((pFirstPath = getFirstFilePath(pApp->pFiles)))
  {
    // Display the files list in ListBrowser
    SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                   pApp->pIntuiWindow, NULL,
                   LISTBROWSER_Labels, (ULONG)pApp->pFiles,
                   LISTBROWSER_AutoFit, TRUE,
                   TAG_DONE);

    // Apply the file path for this session
    strncpy(pApp->FilesPath, pFirstPath, MAXPATHLEN);
    addNotification(pApp->pNotifications, NNT_SELECTED_PATH_INFO, pFirstPath);
  }

  updateNewNames(pApp);
  updateApplicationWindowTitle(pApp);
  notifyUserAboutSkippedFiles(pApp);
}

BOOL updateNewNames(Application* pApp)
{
  BOOL wasUpdatedSuccessfully = TRUE;
  struct Node* pNode;
  STRPTR pName, pExt;
  FileNode* pFileNode;
  LONG counterStart, counterStep, counterPlacesId, counterPlacesValue;
  STRPTR pTextOk = "Ok";
  STRPTR pTextTruncated = "Trunc";
  STRPTR pTextCommandError = "Cmd";
  STRPTR pStateText;

  // Detach list from ListBrowser. Must be done before changing the list.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, ~0,
                  TAG_DONE);

  // Read current name and extension masks
  GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STR_NAME], (ULONG*)&pName);
  GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STR_EXTENSION], (ULONG*)&pExt);
  GetAttr(INTEGER_Number, m_ppGadgets[GID_INT_COUNTER_START], (ULONG*)&counterStart);
  GetAttr(INTEGER_Number, m_ppGadgets[GID_INT_COUNTER_STEP], (ULONG*)&counterStep);
  GetAttr(CHOOSER_Selected, m_ppGadgets[GID_CHO_COUNTER_PLACES], (ULONG*)&counterPlacesId);

  counterPlacesValue = atoi(m_ppCounterPlaces[counterPlacesId]);

  // Use the rename algorithm to fill the NewName fields according the
  // masks and counter settings
  if(createNewNames(pApp->pFiles,
                    pName,
                    pExt,
                    counterStart,
                    counterStep,
                    counterPlacesValue))
  {
    // Set the updated NewName text for each ListBrowser node
    for(pNode = pApp->pFiles->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      pFileNode = (FileNode*)pNode;
      
      if(pFileNode->IsNewNameTruncated)
      {
        pStateText = pTextTruncated;
        wasUpdatedSuccessfully = FALSE;
      }
      else
      {
        pStateText = pTextOk;
      }

      SetListBrowserNodeAttrs(pNode,
                              LBNA_Column, 0,
                                LBNCA_Text, pStateText,
                              LBNA_Column, 2,
                                LBNCA_Text, pFileNode->NewName,
                              TAG_DONE);
    }
  }
  else
  {
    // createNewNames() failed.
    // Set <Error!> for every ListBrowser nodes NewName column.
    for(pNode = pApp->pFiles->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      pFileNode = (FileNode*)pNode;
      SetListBrowserNodeAttrs(pNode,
                              LBNA_Column, 0,
                                LBNCA_Text, pTextCommandError,
                              LBNA_Column, 2,
                                LBNCA_Text, "<Error!>",
                              TAG_DONE);

    }

    wasUpdatedSuccessfully = FALSE;
  }

  // Attach changed list to ListBrowser.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, (ULONG)pApp->pFiles,
                  TAG_DONE);

  // De-/activate Start button depending if all names were updated
  // successfully
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_BTN_START],
                  pApp->pIntuiWindow, 
                  NULL,
                  GA_DISABLED, !wasUpdatedSuccessfully,
                  TAG_DONE);

  return wasUpdatedSuccessfully;
}

#define MAX_CMD_PART_LEN 12

int insertPartIntoString(STRPTR pDest,
                         STRPTR pSrcStr,
                         UBYTE insertPos,
                         char insertCmd,
                         UBYTE insertFrom,
                         UBYTE insertTo)
{
  char commandPartBuf[12];

  if(!pDest || ! pSrcStr || (insertPos < 0)
  || (insertFrom > MAXNAMELEN) || (insertTo > MAXNAMELEN) 
  || (insertFrom > insertTo))
  {
    return -1;
  }

  if((strlen(pSrcStr) + MAX_CMD_PART_LEN) > MAXNAMELEN)
  {
    return -1;
  }

  // Start with a clean target buffer
  strcpy(pDest, "");

  // Apply the beginning until the insert position
  strncat(pDest, pSrcStr, insertPos);
  pDest[insertPos] = '\0';

  // Fill the command buf
  sprintf(commandPartBuf, "[%c%d-%d]", insertCmd, insertFrom, insertTo);

  // Apply the command buf
  strcat(pDest, commandPartBuf);
  
  // Apply the end, after the insert position
  strcat(pDest, pSrcStr + insertPos);

  return (int)(insertPos + strlen(commandPartBuf));
}


void applySelectedRange(Application* pApp)
{
  STRPTR pText;
  long bufferPos;

  if(pApp->ScratchBuf[0] == 'N')
  {
    if(!GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STR_NAME], (ULONG*)&pText))
    {
      printf("Got no STRINGA_TextVal\n");
      return;
    }

    bufferPos = strlen(pText);
    if(0 > (bufferPos = insertPartIntoString(pApp->ScratchBuf,
                                   pText,
                                   bufferPos,
                                   'N',
                                   pApp->pRangeSelectWindow->RangeFrom,
                                   pApp->pRangeSelectWindow->RangeTo)))
    {
      // TODO: Notify user
      printf("insertPartIntoString() failed.\n");
      return;
    }

    SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STR_NAME],
                   pApp->pIntuiWindow,
                   NULL,
                   STRINGA_BufferPos, (ULONG) bufferPos,
                   STRINGA_TextVal, (ULONG) pApp->ScratchBuf,
                   TAG_DONE);
  }
}



static void handleGadgets(Application* pApp, ULONG result)
{
  FileNode* pFileNode;
  switch ((result & WMHI_GADGETMASK))
  {
    case GID_INT_COUNTER_START:
    case GID_INT_COUNTER_STEP:
    case GID_CHO_COUNTER_PLACES:
    case GID_STR_EXTENSION:
    case GID_STR_NAME:
    {
      updateNewNames(pApp);
      break;
    }
    case GID_BTN_NAME:
    {
      appendTextToStrGadget(pApp->pIntuiWindow,
                            m_ppGadgets[GID_STR_NAME],
                            "[N]",
                            pApp->ScratchBuf,
                            SCRATCH_BUF_SIZE); 
      updateNewNames(pApp);
      break;
    }
    case GID_BTN_NAME_PART:
    {
      if((pFileNode = getLongestOldNameNode(pApp->pFiles)))
      {
        // Mark operation for `applySelectedRange()` which will be
        // called when the range select window is closed positively with
        // its Apply/ok button.
        pApp->ScratchBuf[0] = 'N';
        openRangeSelectWindow(pApp->pRangeSelectWindow,
                              pApp->pIntuiWindow,
                              &pApp->SigMask,
                              pFileNode->OriginalName,
                              pFileNode->OriginalNameLen);
      }
      break;
    }
    case GID_BTN_NAME_DATE:
    {
      appendTextToStrGadget(pApp->pIntuiWindow,
                            m_ppGadgets[GID_STR_NAME],
                            "[YMD]",
                            pApp->ScratchBuf,
                            SCRATCH_BUF_SIZE); 
      updateNewNames(pApp);
      break;
    }
    case GID_BTN_NAME_TIME:
    {
      appendTextToStrGadget(pApp->pIntuiWindow,
                            m_ppGadgets[GID_STR_NAME],
                            "[hms]",
                            pApp->ScratchBuf,
                            SCRATCH_BUF_SIZE); 
      updateNewNames(pApp);
      break;
    }

  }
}

void intuiEventLoop(Application* pApp)
{
  ULONG receivedSig;
  ULONG result;
  ULONG code;
  BOOL end = FALSE;

  GetAttr(WINDOW_SigMask, pApp->pWinObject, &pApp->SigMask);

  while (!end)
  {
    receivedSig = Wait(pApp->SigMask);

    // Handle the events of the range select window (if it is open)
    if(TRUE == handleRangeSelectWindowEvents(pApp->pRangeSelectWindow))
    {
      applySelectedRange(pApp);
      updateNewNames(pApp);
    }

    // Handle the events of this (main) window
    while ((result = DoMethod(pApp->pWinObject, WM_HANDLEINPUT, &code)))
    {
      switch (result & WMHI_CLASSMASK)
      {
        case WMHI_CLOSEWINDOW:
          end = TRUE;
          break;
        case WMHI_GADGETUP:
          handleGadgets(pApp, result);
          break;
      }
    }
  }
}


static ULONG myCompare(struct Hook *pHook, Object *pObj, struct LBSortMsg *pMsg)
{
  return 0;
}


Object* createLayout(void)
{
  Object *pMainLayout = NULL, *pTopParentHLayout = NULL, 
         *pTopVLayoutName = NULL, *pTopVLayoutExt = NULL,
         *pTopVLayoutCnt = NULL;

  // Initialize CompareHook for sorting the "Old name" column
  m_CompareHook.h_Entry = (ULONG (*)()) myCompare;
  m_CompareHook.h_SubEntry = NULL;
  m_CompareHook.h_Data = NULL;

  m_pColumnInfo = AllocLBColumnInfo(3,
                                    LBCIA_Column, 0,
                                      LBCIA_Flags, CIF_WEIGHTED,
                                      LBCIA_Sortable, FALSE,
                                      LBCIA_Title, "State",
                                      LBCIA_Weight, 20,
                                    LBCIA_Column, 1,
                                      LBCIA_Flags, CIF_WEIGHTED,
                                      LBCIA_AutoSort, TRUE,
                                      LBCIA_SortArrow, TRUE,
                                      LBCIA_SortDirection, LBMSORT_FORWARD,
                                      LBCIA_Title, "Old name",
                                      LBCIA_Weight, 40,
                                    LBCIA_Column, 2,
                                      LBCIA_Flags, CIF_WEIGHTED,
                                      LBCIA_Sortable, FALSE,
                                      LBCIA_Title, "New name",
                                      LBCIA_Weight, 40,
                                    TAG_DONE);

  pTopVLayoutName = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Name",
    LAYOUT_AddChild, m_ppGadgets[GID_STR_NAME] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STR_NAME,
      GA_RelVerify, TRUE,
      STRINGA_TextVal, (ULONG)"[N]",
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, m_ppGadgets[GID_BTN_NAME] = NewObject(BUTTON_GetClass(), NULL,
        GA_ID, GID_BTN_NAME,
        GA_RelVerify, TRUE,
        GA_Text, (ULONG)"[N] Name",
      TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_BTN_NAME_DATE] = NewObject(BUTTON_GetClass(), NULL,
        GA_Text, (ULONG)"[YMD] Date",
        GA_ID, GID_BTN_NAME_DATE,
        GA_RelVerify, TRUE,
      TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, m_ppGadgets[GID_BTN_NAME_PART] = NewObject(BUTTON_GetClass(), NULL,
        GA_ID, GID_BTN_NAME_PART,
        GA_RelVerify, TRUE,
        GA_Text, (ULONG)"[N#-#] Part",
      TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_BTN_NAME_TIME] = NewObject(BUTTON_GetClass(), NULL,
        GA_ID, GID_BTN_NAME_TIME,
        GA_RelVerify, TRUE,
        GA_Text, (ULONG)"[hms] Time",
      TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, m_ppGadgets[GID_BTN_NAME_COUNTER] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_NAME_COUNTER,
      GA_RelVerify, TRUE,
      GA_Text, (ULONG)"[C] Counter",
    TAG_DONE),
  TAG_DONE);

  pTopVLayoutExt = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Extension",
    LAYOUT_AddChild, m_ppGadgets[GID_STR_EXTENSION] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STR_EXTENSION,
      GA_RelVerify, TRUE,
      STRINGA_TextVal, (ULONG)"[E]",
    TAG_DONE),
    LAYOUT_AddChild, m_ppGadgets[GID_BTN_EXTENSION] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_EXTENSION,
      GA_RelVerify, TRUE,
      GA_Text, (ULONG)"[E] Ext.",
    TAG_DONE),
    LAYOUT_AddChild, m_ppGadgets[GID_BTN_EXTENSION_PART] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_EXTENSION_PART,
      GA_RelVerify, TRUE,
      GA_Text, (ULONG)"[E#-#] Part",
    TAG_DONE),
    LAYOUT_AddChild, m_ppGadgets[GID_BTN_EXTENSION_COUNTER] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_EXTENSION_COUNTER,
      GA_RelVerify, TRUE,
      GA_Text, (ULONG)"[C] Counter",
    TAG_DONE),
  TAG_DONE),

  pTopVLayoutCnt = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Define counter",
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, m_ppGadgets[GID_INT_COUNTER_START] = NewObject(INTEGER_GetClass(), NULL,
        GA_ID, GID_INT_COUNTER_START,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        INTEGER_Number, 1,
        INTEGER_MaxChars, 2,
        INTEGER_Minimum, 0,
        INTEGER_Maximum, 10,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"Start:", TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, m_ppGadgets[GID_INT_COUNTER_STEP] = NewObject(INTEGER_GetClass(), NULL,
        GA_ID, GID_INT_COUNTER_STEP,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        INTEGER_Number, 1,
        INTEGER_MaxChars, 2,
        INTEGER_Minimum, 1,
        INTEGER_Maximum, 10,
      TAG_DONE),
      LABEL_Text, (ULONG)"Step",
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"Step:", TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, m_ppGadgets[GID_CHO_COUNTER_PLACES] = NewObject(CHOOSER_GetClass(), NULL,
        GA_ID, GID_CHO_COUNTER_PLACES,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        CHOOSER_LabelArray, (ULONG)m_ppCounterPlaces,
        CHOOSER_Justification, CHJ_RIGHT,
        CHOOSER_Selected, 1,
        CHOOSER_AutoFit, TRUE,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"Places:", TAG_DONE),
    TAG_DONE),
  TAG_DONE);

  pTopParentHLayout = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
    LAYOUT_SpaceOuter, FALSE,
    LAYOUT_AddChild, pTopVLayoutName,
    CHILD_WeightedWidth, 70,
    LAYOUT_AddChild, pTopVLayoutExt,
    CHILD_WeightedWidth, 30,
    LAYOUT_AddChild, pTopVLayoutCnt,
    CHILD_WeightedWidth, 0,
  TAG_DONE);

  pMainLayout = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    ICA_TARGET, ICTARGET_IDCMP, /* TODO: Remove this?? */
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_DeferLayout, TRUE,  /* this tag instructs layout.gadget to
                                * defer GM_LAYOUT and GM_RENDER and ask
                                * the application to do them. This
                                * lessens the load on input.device
                                */
    LAYOUT_AddChild, (ULONG)pTopParentHLayout,
    CHILD_WeightedHeight, 0,
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_BevelStyle, BVS_GROUP,
      LAYOUT_Label, (ULONG)"Processing list",
      LAYOUT_AddChild, m_ppGadgets[GID_LBR_PROCESSING_LIST] = NewObject(LISTBROWSER_GetClass(), NULL,
        GA_ID, GID_LBR_PROCESSING_LIST,
        GA_RelVerify, TRUE,
        LISTBROWSER_ColumnInfo, (ULONG)m_pColumnInfo,
        LISTBROWSER_ColumnTitles, TRUE,
        LISTBROWSER_TitleClickable, TRUE,
        LISTBROWSER_AutoFit, TRUE,
        LISTBROWSER_HorizontalProp, TRUE,
      TAG_DONE),
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
        LAYOUT_AddChild, m_ppGadgets[GID_BTN_START] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_START,
          GA_RelVerify, TRUE,
          GA_Text, (ULONG)"Start",
        TAG_DONE),
        CHILD_WeightedWidth, 0,
      TAG_DONE),
      CHILD_WeightedHeight, 0,
    TAG_DONE),
  TAG_DONE);

  return pMainLayout;
}

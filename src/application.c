#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
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
  #include <proto/string.h>
  #include <proto/window.h>
#endif

#include <string.h>
#include <stdio.h>

#include "file_list.h"
#include "range_select_window.h"
#include "rename_algorithm.h"
#include "notifications.h"
#include "requester.h"
#include "application.h"


/// Private function declarations

/**
 * Try to add one single file to the ListBrowser.
 */
BOOL addFileToListBrowser(Application* pApp, STRPTR pFileFullPath);

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
void updateNewNames(Application* pApp);

/**
 * Informs the user about error / skip notifications, if there are some.
 * With the option to display the details.
 */
void notifyUserAboutSkippedFiles(Application* pApp);

/**
 * Fill given pTargetBuf by inserting a command like [N12-16] at insert
 * position into given pSrcStr. pSrcStr is not changed, only part wise
 * copied into pTargetBuf.
 *
 * The inserted command is created of the insertCmd character, which can
 * be every char, but only 'N' and 'E' are interpret by the caller for
 * now, and the range insertFrom and insertTo.
 */
int insertPart(STRPTR pTargetBuf,
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
 * Layout creation of main window.
 */
Object* createLayout(void);


/// Private variables

struct ColumnInfo *m_pColumnInfo = NULL;
struct Hook m_CompareHook;

enum gadids
{
    GID_STRING_NAME = 1
  , GID_BTN_NAME
  , GID_BTN_NAME_PART
  , GID_BTN_NAME_DATE
  , GID_BTN_NAME_TIME
  , GID_BTN_NAME_COUNTER
  , GID_STRING_EXTENSION
  , GID_BTN_EXTENSION
  , GID_BTN_EXTENSION_PART
  , GID_BTN_EXTENSION_COUNTER
  , GID_INTEGER_COUNTER_START
  , GID_INTEGER_COUNTER_STEP
  , GID_CHOOSER_COUNTER_PLACES
  , GID_LISTBROWSER
  , GID_BTN_START
  , MAXGADGETS
};

static Object* m_ppGadgets[MAXGADGETS];
struct Hook apphook;

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
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, ~0,
                  TAG_DONE);

  // Add the files of the args if possible
  for(i = 0; i < pMsg->am_NumArgs; i++)
  {
    pFileName = pWbArg[i].wa_Name;
    if(NameFromLock(pWbArg[i].wa_Lock,
                    pApp->pParsedArgs->pScratchPathBuf,
                    MAXPATHLEN))
    {
      AddPart(pApp->pParsedArgs->pScratchPathBuf, pFileName, MAXPATHLEN);
      appendFileNode(pApp->pFileList,
                     pApp->pParsedArgs->pScratchPathBuf,
                     pApp->pNotificationsList);
    }
    else
    {
      if(IoErr() == ERROR_LINE_TOO_LONG)
      {
        // For the error notification only the file name not the
        // relative path is needed.
        addNotification(pApp->pNotificationsList,
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
    if((pApp->pAppWindowPort = CreateMsgPort()))
    {
      if((pApp->pNotificationsList = createNotificationList()))
      {
        if((pApp->pFileList = createFileList()))
        {
          if((pApp->pParsedArgs = createParsedArgs(argc,
                                                  argv,
                                                  pApp->pFileList,
                                                  pApp->pNotificationsList)))
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
                                              WINDOW_AppMsgHook, &apphook,
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

  if(pApp->pFileList)
  {
    freeFileList(pApp->pFileList);
  }

  if(pApp->pNotificationsList)
  {
    freeNotificationList(pApp->pNotificationsList);
  }

  if(pApp->pAppWindowPort)
  {
    DeleteMsgPort(pApp->pAppWindowPort);
  }

  FreeVec(pApp);
}

BOOL runApplication(Application* pApp)
{
  if(!pApp)
  {
    return FALSE;
  }

  apphook.h_Entry = (ULONG (* )())AppMsgFunc;
  apphook.h_SubEntry = NULL;
  apphook.h_Data = pApp;


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
  if(containsSkippedNotifications(pApp->pNotificationsList))
  {
    if(!showEasyRequest(pApp->pIntuiWindow,
                        "Continue|Show errors",
                        "Failed to add some of the input files"))
    {
      printNotifications(pApp->pNotificationsList);
      clearNotificationsExcept(pApp->pNotificationsList,
                                NNT_SELECTED_PATH_INFO);
    }
  }
}

void applyNewFiles(Application* pApp)
{
  STRPTR pFirstPath;

  // Does list contain at least one file?
  if((pFirstPath = getFirstFilePath(pApp->pFileList)))
  {
    // Display the files list in ListBrowser
    SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                   pApp->pIntuiWindow, NULL,
                   LISTBROWSER_Labels, (ULONG)pApp->pFileList,
                   TAG_DONE);

    // Apply the file path for this session
    strncpy(pApp->FilesPath, pFirstPath, MAXPATHLEN);
    addNotification(pApp->pNotificationsList, NNT_SELECTED_PATH_INFO, pFirstPath);
  }

  updateNewNames(pApp);
  updateApplicationWindowTitle(pApp);
  notifyUserAboutSkippedFiles(pApp);
}

void updateNewNames(Application* pApp)
{
  struct Node* pNode;
  STRPTR pName, pExt;
  FileNode* pFileNode;
  LONG counterStart, counterStep;
  WORD counterPlacesId;

  // Detach list from ListBrowser. Must be done before changing the list.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, ~0,
                  TAG_DONE);

  // Read current name and extension masks
  GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STRING_NAME], (ULONG*)&pName);
  GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STRING_EXTENSION], (ULONG*)&pExt);
  GetAttr(INTEGER_Number, m_ppGadgets[GID_INTEGER_COUNTER_START], (ULONG*)&counterStart);
  GetAttr(INTEGER_Number, m_ppGadgets[GID_INTEGER_COUNTER_STEP], (ULONG*)&counterStep);
  // GetAttr(CHOOSER_Selected, m_ppGadgets[GID_CHOOSER_COUNTER_PLACES], (ULONG*)&counterPlacesId);
// printf("id = %d\n", counterPlacesId);
  // Use the rename algorithm to fill the NewName fields according the
  // masks and counter settings
  createNewNames(pApp->pFileList,
                 pName,
                 pExt,
                 counterStart,
                 counterStep,
                 2);

  // Set the updated NewName text for each ListBrowser node
  for(pNode = pApp->pFileList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    SetListBrowserNodeAttrs(pNode,
                            LBNA_Column, 1,
                              LBNCA_Text, pFileNode->NewName,
                            TAG_DONE);
  }

  // Attach changed list to ListBrowser.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, (ULONG)pApp->pFileList,
                  TAG_DONE);
}

BOOL addFileToListBrowser(Application* pApp, STRPTR pFileFullPath)
{
  BOOL result;

  // Detach list from ListBrowser. Must be done before changing the list.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, ~0,
                  TAG_DONE);

  result = appendFileNode(pApp->pFileList,
                          pFileFullPath,
                          pApp->pNotificationsList);

  // Attatch changed list to ListBrowser.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, (ULONG)pApp->pFileList,
                  TAG_DONE);

  return result;
}

ULONG myBufferPos = -1;
ULONG myGotBufferPosAttr;

static void handleGadgets(Application* pApp, ULONG result)
{
  FileNode* pFileNode;
  switch ((result & WMHI_GADGETMASK))
  {
    case GID_INTEGER_COUNTER_START:
    case GID_INTEGER_COUNTER_STEP:
    case GID_STRING_EXTENSION:
    case GID_STRING_NAME:
    {
      // myGotBufferPosAttr = GetAttr(STRINGA_BufferPos, m_ppGadgets[GID_STRING_NAME], &myBufferPos);
      // if(myGotBufferPosAttr == 1)
      // {
      //   printf("Successfully got BufferPosAttr: '%d'\n", myBufferPos);
      // }
      // else
      // {
      //   printf("FAILED to get BufferPosAttr. (bufferPos value is: %d)\n", myBufferPos);
      // }

      updateNewNames(pApp);
      break;
    }
    case GID_BTN_NAME:
    {
      break;
    }
    case GID_BTN_NAME_PART:
    {
      if((pFileNode = getLongestOldNameNode(pApp->pFileList)))
      {
        // Mark operation for `applySelectedRange()` which will be
        // called when the range select window is closed positively with
        // its Apply/ok button.
        pApp->ScratchBuf[0] = 'N';
        openRangeSelectWindow(pApp->pRangeSelectWindow,
                              pApp->pIntuiWindow,
                              &pApp->SigMask,
                              pFileNode->OldName);
      }

      break;
    }
    case GID_BTN_NAME_DATE:
    {
      // TODO: Remove after testing/debugging. Changes new names!
      addFileToListBrowser(pApp, "Shared:dev/projects/MultiRename/testdata/My_3rd_attempt.doc");
      notifyUserAboutSkippedFiles(pApp);
      break;
    }
  }
}


#define MAX_CMD_PART_LEN 12


int insertPart(STRPTR pTargetBuf,
               STRPTR pSrcStr,
               UBYTE insertPos,
               char insertCmd,
               UBYTE insertFrom,
               UBYTE insertTo)
{
  char commandPartBuf[12];

  if(!pTargetBuf || ! pSrcStr || (insertPos < 0)
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
  strcpy(pTargetBuf, "");

  // Apply the beginning until the insert position
  strncat(pTargetBuf, pSrcStr, insertPos);
  pTargetBuf[insertPos] = '\0';

  // Fill the command buf
  sprintf(commandPartBuf, "[%c%d-%d]", insertCmd, insertFrom, insertTo);

  // Apply the command buf
  strcat(pTargetBuf, commandPartBuf);
  
  // Apply the end, after the insert position
  strcat(pTargetBuf, pSrcStr + insertPos);

  return insertPos + strlen(commandPartBuf);
}


void applySelectedRange(Application* pApp)
{
  STRPTR pText;
  ULONG bufferPos;
  ULONG gotTextValAttr, gotBufferPosAttr;

  if(pApp->ScratchBuf[0] == 'N')
  {
    gotTextValAttr = GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STRING_NAME], (ULONG*)&pText);
    gotBufferPosAttr = GetAttr(STRINGA_BufferPos, m_ppGadgets[GID_STRING_NAME], &bufferPos);

    if(gotTextValAttr == 1)
    {
      printf("Successfully got TextValAttr: '%s'\n", pText);
    }
    else
    {
      printf("FAILED to get TextValAttr.\n");
    }

    if(gotBufferPosAttr == 1)
    {
      printf("Successfully got BufferPosAttr: '%d'\n", bufferPos);
    }
    else
    {
      printf("FAILED to get BufferPosAttr. (bufferPos value is: %d)\n", bufferPos);
    }
printf("ranegFrom = %d, rangeTo = %d\n", pApp->pRangeSelectWindow->RangeFrom, pApp->pRangeSelectWindow->RangeTo);
    if(0 > (bufferPos = insertPart(pApp->ScratchBuf,
                                   pText,
                                   bufferPos,
                                   'N',
                                   pApp->pRangeSelectWindow->RangeFrom,
                                   pApp->pRangeSelectWindow->RangeTo)))
    {
      // TODO: Notify user
      return;
    }

printf("pScratchBuf = '%s'\n", pApp->ScratchBuf);

    SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STRING_NAME],
                   pApp->pIntuiWindow,
                   NULL,
                   STRINGA_BufferPos, (ULONG) bufferPos,
                   STRINGA_TextVal, (ULONG) pApp->ScratchBuf,
                   TAG_DONE);
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

static UBYTE *ppCounterPlaces[] = { "1", "2", "3", "4", "5",
                                    "6", "7", "8", "9", "10", NULL };

Object* createLayout(void)
{
  Object *pMainLayout = NULL, *pTopParentHLayout = NULL, 
         *pTopVLayoutName = NULL, *pTopVLayoutExt = NULL,
         *pTopVLayoutCnt = NULL;

  // Initialize CompareHook for sorting the "Old name" column
  m_CompareHook.h_Entry = (ULONG (*)()) myCompare;
  m_CompareHook.h_SubEntry = NULL;
  m_CompareHook.h_Data = NULL;

  m_pColumnInfo = AllocLBColumnInfo(2,
                                    LBCIA_Column, 0,
                                    LBCIA_Flags, CIF_WEIGHTED,
                                    LBCIA_AutoSort, TRUE,
                                    LBCIA_SortArrow, TRUE,
                                    LBCIA_SortDirection, LBMSORT_FORWARD,
                                    LBCIA_Title, "Old name",
                                    LBCIA_Weight, 50,
                                    LBCIA_Column, 1,
                                    LBCIA_Flags, CIF_WEIGHTED,
                                    LBCIA_Sortable, FALSE,
                                    LBCIA_Title, "New name",
                                    LBCIA_Weight, 50,
                                    TAG_DONE);

  pTopVLayoutName = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Name",
    LAYOUT_AddChild, m_ppGadgets[GID_STRING_NAME] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STRING_NAME,
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
    LAYOUT_AddChild, m_ppGadgets[GID_STRING_EXTENSION] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STRING_EXTENSION,
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
      LAYOUT_AddChild, m_ppGadgets[GID_INTEGER_COUNTER_START] = NewObject(INTEGER_GetClass(), NULL,
        GA_ID, GID_INTEGER_COUNTER_START,
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
      LAYOUT_AddChild, m_ppGadgets[GID_INTEGER_COUNTER_STEP] = NewObject(INTEGER_GetClass(), NULL,
        GA_ID, GID_INTEGER_COUNTER_STEP,
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
      LAYOUT_AddChild, m_ppGadgets[GID_CHOOSER_COUNTER_PLACES] = NewObject(CHOOSER_GetClass(), NULL,
        GA_ID, GID_CHOOSER_COUNTER_PLACES,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        CHOOSER_LabelArray, (ULONG)ppCounterPlaces,
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
      LAYOUT_AddChild, m_ppGadgets[GID_LISTBROWSER] = NewObject(LISTBROWSER_GetClass(), NULL,
        GA_ID, GID_LISTBROWSER,
        GA_RelVerify, TRUE,
        LISTBROWSER_ColumnInfo, (ULONG)m_pColumnInfo,
        LISTBROWSER_ColumnTitles, TRUE,
        LISTBROWSER_TitleClickable, TRUE,
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

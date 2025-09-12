#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
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

#include "file_nodes.h"
#include "file_tools.h"
#include "notifications.h"
#include "range_select_window.h"
#include "rename.h"
#include "rename_algorithm.h"
#include "requester.h"
#include "string_tools.h"
#include "ui_tools.h"
#include "application.h"

/// Defines

#define VERSTAG "\0$VER: MultiRename 0.5 (22.3.2025)"
#define COPYRIGHT "\n\nCopyright(c) 2024 Uwe Rosner (u.rosner@ymail.com)\n\n"
#define DISTRIBUTION "This release of MultiRename may be freely distributed.\n" \
                     "It may not be commercially distributed without the\n" \
                     "explicit permission of the author.\n"

///
/// Forwards / private function declarations

BOOL startRename(Application* pApp);

/**
 * Iterate the given array of WbArgs and (try to) add each file to the
 * application processing list.
 *
 * NOTE: Detaches the list browser labels and the it calls
 * `applyNewFiles()` which then attaches them again.
 */
void appendFilesByWbArgs(Application* pApp, struct WBArg *pArgs, ULONG numArgs);


/**
 * Re-attaches the list of FileNodes to the list browser.
 * Set the current working path if necessary.
 * Updates window title with the current working path.
 * 
 * NOTE: Attaches the list browser labels. Must be detached before this call!
 */
void applyNewFiles(Application* pApp);

/**
 * When range selection window was closed with Ok, the resulting mask is
 * inserted in current cursor position.
 */
BOOL applySelectedRange(Application* pApp);

/**
 * Set the current working path as application window title.
 */
void updateMainWindowTitle(Application* pApp);

/**
 * Enables or disables the main windows gadgets depending on the state
 */
void setAllGadgetsDisabledState(Application* pApp, BOOL disable);

/**
 * Calculates the new names in the processing list / ListBrowser.
 * NOTE: De- and attaches the list browser labels.
 */
BOOL updateNewNames(Application* pApp);

/**
 * Informs the user about error / skip notifications, if there are some.
 * With the option to display the details.
 */
void notifyUserAboutSkippedFiles(Application* pApp);

/**
 * The application event loop.
 */
void intuiEventLoop(Application* pApp);

/**
 * Create layout for main window.
 */
Object* createLayout(void);

///
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
struct Hook m_AppHook;


///
/// Helper implementation
struct NewMenu longFileNamesItem =  { NM_ITEM, "Allow long filenames", 0 , CHECKIT|MENUTOGGLE, 0, NULL};
struct NewMenu skipIconsItem =      { NM_ITEM, "Skip icons",           0 , CHECKIT|MENUTOGGLE, 0, NULL};

enum
{ 
  MENU_PROJECT_NEW = 1,
  MENU_PROJECT_ADD_FILES,
  MENU_PROJECT_ABOUT,
  MENU_PROJECT_QUIT,
  MENU_SETTINGS_LONGNAMES,
  MENU_SETTINGS_SKIPICONS,
 };

struct NewMenu mainWindowNewMenu[] =
{
  { NM_TITLE,   "Project",                 0 , 0,                  0, NULL},
  {   NM_ITEM,    "New",                  "n", 0,                  0, (APTR) MENU_PROJECT_NEW},
  {   NM_ITEM,    "Add files...",         "a", 0,                  0, (APTR) MENU_PROJECT_ADD_FILES},
  {   NM_ITEM,    NM_BARLABEL,             0 , 0,                  0, NULL},
  {   NM_ITEM,    "About",                 0 , 0,                  0, (APTR) MENU_PROJECT_ABOUT},
  {   NM_ITEM,    NM_BARLABEL,             0 , 0,                  0, NULL},
  {   NM_ITEM,    "Quit",                 "q", 0,                  0, (APTR) MENU_PROJECT_QUIT},
  { NM_TITLE,   "Settings",                0 , 0,                  0, NULL},
  {   NM_ITEM,    "Allow long filenames",  0 , CHECKIT|MENUTOGGLE, 0, (APTR) MENU_SETTINGS_LONGNAMES},
  {   NM_ITEM,    "Skip icons",            0 , CHECKIT|MENUTOGGLE, 0, (APTR) MENU_SETTINGS_SKIPICONS},
  { NM_END, NULL, NULL, 0, 0, NULL}
};

struct NewMenu* findNewMenuItem(struct NewMenu* pNewMenuArray, ULONG itemId)
{
  ULONG i = 0;
  
  if(!pNewMenuArray)
  {
    return NULL;
  }

  while(pNewMenuArray[i].nm_Type != NM_END)
  {
    if((ULONG)pNewMenuArray[i].nm_UserData == itemId)
    {
      return &pNewMenuArray[i];
    }

    i++;
  }

  return NULL;
}


STRPTR createAboutMessage(void)
{
  STRPTR pAboutMsg;
  ULONG totalLength = strlen(VERSTAG + 7)
                    + strlen(COPYRIGHT)
                    + strlen(DISTRIBUTION) + 1;

  if(!(pAboutMsg = AllocVec(totalLength * sizeof(char), MEMF_CLEAR)))
  {
    return NULL;
  }

  strcpy(pAboutMsg, VERSTAG + 7);
  strcat(pAboutMsg, COPYRIGHT);
  strcat(pAboutMsg, DISTRIBUTION);
  
  return pAboutMsg;
}

/**
 * In `createMainWindow` this function is set to a hook to be called by
 * Intuition/BOOPSI when app messages are received. These messages can
 * contain some WbArgs, e.g. files that have been dragged to app window.
 * This function adds these WbArgs/files to the processing list.
 */
void __ASM__ __SAVE_DS__ AppMsgFunc(__REG__(a0, struct Hook *pHook),
                                    __REG__(a2, Object *pWindow),
                                    __REG__(a1, struct AppMessage *pMsg))
{
  Application* pApp = (Application*)pHook->h_Data;
  appendFilesByWbArgs(pApp, pMsg->am_ArgList, pMsg->am_NumArgs);
}



Object* createMainWindow(Application* pApp, Object* pMainWindowLayout)
{
  struct NewMenu* pNewMenuItem;
  ULONG screenBarHeight;
  // Calculate an alternative size of the window for when
  // the ZOOM gadget is clicked. It should use the whole
  // screen minus the screen title bar.
  UWORD zoomData[] = { 0, 20, 320, 256};
  Object* pWindowObject;

  if(!pApp || !pApp->pParsedArgs || !pMainWindowLayout)
  {
    return NULL;
  }

  if(pApp->pParsedArgs->AreLongNamesAllowed)
  {
    if((pNewMenuItem = findNewMenuItem(mainWindowNewMenu, MENU_SETTINGS_LONGNAMES)))
    {
      pNewMenuItem->nm_Flags |= CHECKED;
    }
  }

  if(pApp->pParsedArgs->AreIconsSkipped)
  {
    if((pNewMenuItem = findNewMenuItem(mainWindowNewMenu, MENU_SETTINGS_SKIPICONS)))
    {
      pNewMenuItem->nm_Flags |= CHECKED;
    }
  }

  if(pApp->pParsedArgs->pPubScreenName)
  {
    if(!(pApp->pPubScreen = LockPubScreen(pApp->pParsedArgs->pPubScreenName)))
    {
      Printf("Failed to lock public screen '%s'\n",
             pApp->pParsedArgs->pPubScreenName);
      return NULL;
    }
  }
  else
  {
    if(!(pApp->pPubScreen = LockPubScreen(NULL)))
    {
      PutStr("Failed to lock default public screen.\n");
      return NULL;
    }
  }
  
  screenBarHeight = pApp->pPubScreen ->BarHeight 
                  + pApp->pPubScreen ->BarVBorder;

  zoomData[1] = screenBarHeight + 1;
  zoomData[2] = pApp->pPubScreen ->Width;
  zoomData[3] = pApp->pPubScreen ->Height - screenBarHeight - 1;

  m_AppHook.h_Entry = (ULONG (* )())AppMsgFunc;
  m_AppHook.h_SubEntry = NULL;
  m_AppHook.h_Data = pApp;

  pWindowObject = NewObject(WINDOW_GetClass(), NULL,
    WINDOW_Position, WPOS_CENTERSCREEN,
    WA_Activate, TRUE,
    WA_Title, "MultiRename",
    WA_CloseGadget, TRUE,
    WA_DepthGadget, TRUE,
    WA_DragBar, TRUE,
    WA_SizeGadget, TRUE,
    WA_Width, 640,
    WA_Height, 480,
    WA_Zoom, (ULONG) zoomData,
    WA_PubScreen, pApp->pPubScreen,
    WA_AutoAdjust, TRUE,
    WA_NewLookMenus, TRUE,
    WA_IDCMP, IDCMP_CLOSEWINDOW
            | IDCMP_GADGETUP
            | IDCMP_NEWSIZE
            | IDCMP_MOUSEBUTTONS
            | IDCMP_ACTIVEWINDOW
            | IDCMP_INACTIVEWINDOW,
    WINDOW_Layout, pMainWindowLayout,
    WINDOW_NewMenu, mainWindowNewMenu,
    WINDOW_AppPort, pApp->pAppWindowPort,
    WINDOW_AppWindow, TRUE,
    WINDOW_AppMsgHook, &m_AppHook,
    TAG_DONE);

  return pWindowObject;
}

///
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
          if((pApp->pFiles = createFileNodes()))
          {
            if((pApp->pParsedArgs = createParsedArgs(argc,
                                                     argv,
                                                     pApp->pFiles,
                                                     pApp->pLocale,
                                                     pApp->pNotifications)))
            {
              if((pMainLayout = createLayout()))
              {
                if((pApp->pWinObject = createMainWindow(pApp, pMainLayout)))
                {
                  if((pApp->pRangeSelectWindow = createRangeSelectWindow()))
                  {
                    if((pApp->pAboutMessage = createAboutMessage()))
                    {
                      // Mark the buffer positions as invalid
                      pApp->NameGadgetBufferPos = -1;
                      pApp->ExtGadgetBufferPos = -1;
                      return pApp;
                    }
                    else
                    {
                      PutStr("Failed to create the about message.\n");
                      disposeApplication(pApp);
                    }
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

  if(pApp->pAboutMessage)
  {
    FreeVec(pApp->pAboutMessage);
  }

  if(pApp->pRangeSelectWindow)
  {
    freeRangeSelectWindow(pApp->pRangeSelectWindow);
  }

  if(pApp->pWinObject)
  {
    DisposeObject(pApp->pWinObject);
  }

  if(pApp->pPubScreen)
  {
    UnlockPubScreen(NULL, pApp->pPubScreen);
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
    freeFileNodes(pApp->pFiles);
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

///
/// Private function implementations
void updateMainWindowTitle(Application* pApp)
{
  // If a files is already set (e.e. a lock exists)
  if(getFilesDirLock(pApp->pFiles))
  {
    strcpy(pApp->WindowTitle, "MultiRename in [");
    strcat(pApp->WindowTitle, getFilesDirPath(pApp->pFiles));
    strcat(pApp->WindowTitle, "]");
  }
  else
  {
    strcpy(pApp->WindowTitle, "MultiRename");
  }
  
  SetWindowTitles(pApp->pIntuiWindow, pApp->WindowTitle, (UBYTE *)~0);
}

void setAllGadgetsDisabledState(Application* pApp, BOOL disable)
{
  ULONG gadgetId = 1;

  if(!pApp)
  {
    return;
  }

  for(gadgetId = 1; gadgetId < MAXGADGETS; gadgetId++)
  {
    if(gadgetId == GID_BTN_START && !disable && !pApp->IsStartAllowed)
    {
      // Skip enabling of the start button if it is not allowed to be
      // enabled.
      continue;
    }

    SetGadgetAttrs((struct Gadget *) m_ppGadgets[gadgetId],
                    pApp->pIntuiWindow, 
                    NULL,
                    GA_DISABLED, disable,
                    TAG_DONE);
  }
}

void notifyUserAboutSkippedFiles(Application* pApp)
{
  if(containsSkippedNotifications(pApp->pNotifications))
  {
    if(!showEasyRequest(pApp->pWinObject, 
                        pApp->pIntuiWindow,
                        "MultiRename",
                        "Ok|Show errors",
                        "Failed to add some of the input files"))
    {
      printNotifications(pApp->pNotifications);
    }

    clearNotificationsExcept(pApp->pNotifications, NNT_SELECTED_PATH_INFO);
  }
}

BOOL startRename(Application* pApp)
{
  ULONG fileCount;
  TokenCount* pTokenCounts;
  struct Node* pNode;
  FileNode* pFileNode;
  BPTR pFormerDirLock = 0L;
  BOOL hasAlreadyAskedToProceed = FALSE;
  BOOL didRenameSucceed = FALSE;
  STRPTR pFileNameExtensionDot = NULL;
  ULONG fileNameExtensionDotIdx;

  fileCount = countFileNodes(pApp->pFiles);
  if(fileCount == 0)
  {
    showEasyRequest(pApp->pWinObject, 
                    pApp->pIntuiWindow,
                    "MultiRename",
                    "Ok",
                    "No files to rename.");
    return FALSE;
  }

  if(!(pTokenCounts = createTokenCounts(fileCount)))
  {
    showEasyRequest(pApp->pWinObject, 
                    pApp->pIntuiWindow,
                    "MultiRename",
                    "Cancel",
                    "Error, failed to create file name tokens!");
    return FALSE;
  }

  fillTokenOccurrences(pApp->pFiles, pTokenCounts, fileCount);

  for(pNode = pApp->pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    if(pFileNode->TokenOccurrenceNumber > 1)
    {
      if(!hasAlreadyAskedToProceed)
      {
        // Construct and display the `double occurrence` error message.
        // Use a 2k temporary buffer that is big enough for the 136 bytes
        // message text + max. 107 bytes file name.
        sprintf(pApp->pParsedArgs->pTempPathBuf,
                "Warning, duplicate names! Proceed anyway?\n" \
                "%s\n\n" \
                "NOTE: Proceed will auto rename duplicate files to \n" \
                "  name (2).ext\n" \
                "  name (3).ext\n" \
                "and so on.",
                pFileNode->NewName);
  
        if(!showEasyRequest(pApp->pWinObject, 
                            pApp->pIntuiWindow,
                            "MultiRename",
                            "Proceed|Cancel",
                            pApp->pParsedArgs->pTempPathBuf))
        {
          // User clicked on `Cancel`
          freeTokenCounts(pTokenCounts);
          return FALSE;
        }

        hasAlreadyAskedToProceed = TRUE;
      }

      // Prepare the number text, for example `(2)`, etc.
      sprintf(pApp->pParsedArgs->pTempPathBuf, " (%d)", pFileNode->TokenOccurrenceNumber);

      // Find the dot '.' in new filename
      if((pFileNameExtensionDot = strrchr(pFileNode->NewName, '.')))
      {
        fileNameExtensionDotIdx = pFileNameExtensionDot - (STRPTR)pFileNode->NewName;
        if(insertString(pFileNode->NewName,
                        pApp->pParsedArgs->pTempPathBuf,
                        fileNameExtensionDotIdx,
                        pApp->TempBuf,
                        TEMP_BUF_SIZE) < 0)
        {
          showEasyRequest(pApp->pWinObject, 
                          pApp->pIntuiWindow,
                          "MultiRename",
                          "Cancel",
                          "Error, failed to automatically create name for duplicate file!");
          freeTokenCounts(pTokenCounts);
          return FALSE;
        }
        
        // Check if name length (+ the possible '.info') is allowed by
        // file system. TODO: Replace MAX_NAME_LEN by proper allowed
        // length 32 || 107
        if((strlen(pApp->TempBuf) + 5) > MAX_NAME_LEN)
        {
          showEasyRequest(pApp->pWinObject, 
                          pApp->pIntuiWindow,
                          "MultiRename",
                          "Cancel",
                          "Error, auto-renamed file name would be " \
                          "too long for file system!");
          freeTokenCounts(pTokenCounts);
          return FALSE;
        }

        strcpy(pFileNode->NewName, pApp->TempBuf);
      }
    }
  }

  // Change to files directory, perform the rename and change back to
  // former directory
  pFormerDirLock = CurrentDir(pApp->pFiles->DirLock);
  if(!(didRenameSucceed = renameFiles(pApp->pFiles,
                                     pApp->pNotifications,
                                     pApp->pParsedArgs->AreIconsSkipped)))
  {
    if(!showEasyRequest(pApp->pWinObject, 
                        pApp->pIntuiWindow,
                        "MultiRename",
                        "Ok|Show errors",
                        "Failed to rename some of the input files"))
    {
      printNotifications(pApp->pNotifications);
      return FALSE;
    }
  }
  
  pApp->IsRenameDone = TRUE;
  CurrentDir(pFormerDirLock);
  freeTokenCounts(pTokenCounts);
  return didRenameSucceed;
}

void appendFilesByWbArgs(Application* pApp, struct WBArg *pArgs, ULONG numArgs)
{
  ULONG i;
  STRPTR pFileName;

  if(numArgs == 0)
  {
    return;
  }

  // Detach list from ListBrowser. Must be done before changing the list.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, ~0,
                  TAG_DONE);

  // Iterate the files of the args and append them as FileNode if possible
  for(i = 0; i < numArgs; i++)
  {
    pFileName = pArgs[i].wa_Name;
    if(NameFromLock(pArgs[i].wa_Lock,
                    pApp->pParsedArgs->pTempPathBuf,
                    MAX_PATH_LEN))
    {
      // Now scratch buf contains the name of the directory of the file
      // So next the fileName is appended to the buf
      AddPart(pApp->pParsedArgs->pTempPathBuf, pFileName, MAX_PATH_LEN);

      appendFileNode(pApp->pFiles,
                     pApp->pParsedArgs->pTempPathBuf,
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

void applyNewFiles(Application* pApp)
{
  STRPTR pFirstPath;
  BPTR lock;

  // Does list contain at least one file?
  if((pFirstPath = getFirstFilePath(pApp->pFiles)))
  {
    if(!getFilesDirLock(pApp->pFiles))
    {
      if((lock = lockFromLongName(pFirstPath)))
      {
        if(!setFilesDirLock(pApp->pFiles, lock))
        {
          addNotification(pApp->pNotifications,
                          NNT_SKIPPED_PATH_TOO_LONG,
                          pFirstPath);
        }
      }
      else
      {
        addNotification(pApp->pNotifications,
                        NNT_SKIPPED_FAILED_LOCK,
                        pFirstPath);
      }
    }

    // Display the files list in ListBrowser
    SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                   pApp->pIntuiWindow, NULL,
                   LISTBROWSER_Labels, (ULONG)pApp->pFiles->pList,
                   LISTBROWSER_AutoFit, TRUE,
                   TAG_DONE);
  }

  updateNewNames(pApp);
  updateMainWindowTitle(pApp);
  setAllGadgetsDisabledState(pApp, pApp->IsRenameDone);
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
  STRPTR pTruncatedLongNoIcons = "> 107";
  STRPTR pTruncatedLong = "> 102";
  STRPTR pTruncatedShortNoIcons = "> 32";
  STRPTR pTruncatedShort = "> 27";
  STRPTR pTextTruncated = NULL;
  STRPTR pTextCommandError = "Cmd";
  STRPTR pStateText;
  long maxAllowedNameLength;

  if(pApp->pParsedArgs->AreLongNamesAllowed)
  {
    if(pApp->pParsedArgs->AreIconsSkipped)
    {
      maxAllowedNameLength = 107;
      pTextTruncated = pTruncatedLongNoIcons;
    }
    else
    {
      maxAllowedNameLength = 102; // 107 - 5 bytes for '.info'
      pTextTruncated = pTruncatedLong;
    }
  }
  else
  {
    if(pApp->pParsedArgs->AreIconsSkipped)
    {
      maxAllowedNameLength = 32;
      pTextTruncated = pTruncatedShortNoIcons;
    }
    else
    {
      maxAllowedNameLength = 27; // 32 - 5 bytes for '.info'
      pTextTruncated = pTruncatedShort;
    }
  }

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
                    maxAllowedNameLength,
                    pName,
                    pExt,
                    counterStart,
                    counterStep,
                    counterPlacesValue))
  {
    // Set the updated NewName text for each ListBrowser node
    for(pNode = pApp->pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
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
                              LBNA_Column, 3,
                                LBNCA_Text, pFileNode->NewName,
                              TAG_DONE);
    }
  }
  else
  {
    // createNewNames() failed.
    // Set <Error!> for every ListBrowser nodes NewName column.
    for(pNode = pApp->pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      pFileNode = (FileNode*)pNode;
      SetListBrowserNodeAttrs(pNode,
                              LBNA_Column, 0,
                                LBNCA_Text, pTextCommandError,
                              LBNA_Column, 3,
                                LBNCA_Text, "<Error!>",
                              TAG_DONE);

    }

    wasUpdatedSuccessfully = FALSE;
  }

  // Attach changed list to ListBrowser.
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                  pApp->pIntuiWindow, 
                  NULL,
                  LISTBROWSER_Labels, (ULONG)pApp->pFiles->pList,
                  TAG_DONE);

  // De-/activate Start button depending if all names were updated
  // successfully
  pApp->IsStartAllowed = wasUpdatedSuccessfully;
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_BTN_START],
                  pApp->pIntuiWindow, 
                  NULL,
                  GA_DISABLED, !pApp->IsStartAllowed,
                  TAG_DONE);

  return wasUpdatedSuccessfully;
}

BOOL applySelectedRange(Application* pApp)
{
  long bufferPos = -1;
  STRPTR pText;
  Object *pStrGadget;

  switch(pApp->RangeMask.RequestedRangeType)
  {
    case RRT_NAME:
      if(!GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STR_NAME], (ULONG*)&pText))
      {
        return FALSE;
      }
      pStrGadget = m_ppGadgets[GID_STR_NAME];
      bufferPos = pApp->NameGadgetBufferPos;
      break;
    case RRT_EXTENSION:
      if(!GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STR_EXTENSION], (ULONG*)&pText))
      {
        return FALSE;
      }
      pStrGadget = m_ppGadgets[GID_STR_EXTENSION];
      bufferPos = pApp->ExtGadgetBufferPos;
      break;
    default:
      return FALSE;
  }

  if(bufferPos == -1)
  {
    bufferPos = strlen(pText);
  }

  if(0 > (bufferPos = insertRangeMaskString(&pApp->RangeMask,
                                            pApp->TempBuf,
                                            TEMP_BUF_SIZE,
                                            pText,
                                            bufferPos)))
  {
    // TODO: Notify user
    printf("insertRangeMaskString() failed.\n");
    return FALSE;
  }

  // First, set the new text into string gadget
  SetGadgetAttrs((struct Gadget *) pStrGadget,
                 pApp->pIntuiWindow,
                 NULL,
                 STRINGA_TextVal, (ULONG) pApp->TempBuf,
                 TAG_DONE);

  // And then set the buffer pos to insert position
  //
  //(Because setting both in one `SetGadgetAttrs` call doesn't work, as
  // `STRINGA_TextVal` always overwrites the buffer pos to the end of
  // line.)
  SetGadgetAttrs((struct Gadget *) pStrGadget,
                 pApp->pIntuiWindow,
                 NULL,
                 STRINGA_BufferPos, (ULONG) bufferPos,
                 TAG_DONE);

  return TRUE;
}

void insertCommandToStrGadget(Application* pApp,
                              ULONG strGadgetId,
                              STRPTR pCommandStr)
{
  pApp->NameGadgetBufferPos = getStrGadgetBufferPos(m_ppGadgets[GID_STR_NAME]);
  pApp->ExtGadgetBufferPos = getStrGadgetBufferPos(m_ppGadgets[GID_STR_EXTENSION]);

  if(strGadgetId == GID_STR_NAME)
  {
    pApp->NameGadgetBufferPos = insertTextToStrGadget(pApp->pIntuiWindow,
                                                      m_ppGadgets[GID_STR_NAME],
                                                      pCommandStr,
                                                      pApp->NameGadgetBufferPos,
                                                      pApp->TempBuf,
                                                      TEMP_BUF_SIZE);
  }
  else if(strGadgetId == GID_STR_EXTENSION)
  {
    pApp->NameGadgetBufferPos = insertTextToStrGadget(pApp->pIntuiWindow,
                                                      m_ppGadgets[GID_STR_EXTENSION],
                                                      pCommandStr,
                                                      pApp->ExtGadgetBufferPos,
                                                      pApp->TempBuf,
                                                      TEMP_BUF_SIZE);
  }

  updateNewNames(pApp);
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
      insertCommandToStrGadget(pApp, GID_STR_NAME, "[N]");
      break;
    }
    case GID_BTN_NAME_PART:
    {
      pApp->NameGadgetBufferPos = getStrGadgetBufferPos(m_ppGadgets[GID_STR_NAME]);
      pApp->ExtGadgetBufferPos = getStrGadgetBufferPos(m_ppGadgets[GID_STR_EXTENSION]);
      if((pFileNode = getLongestOldNameNode(pApp->pFiles)))
      {
        pApp->RangeMask.RequestedRangeType = RRT_NAME;
        openRangeSelectWindow(pApp->pRangeSelectWindow,
                              pApp->pIntuiWindow,
                              &pApp->SigMask,
                              &pApp->RangeMask,
                              pFileNode->OriginalName,
                              pFileNode->OriginalNameLen);
      }
      else
      {
        showEasyRequest(pApp->pWinObject, 
                        pApp->pIntuiWindow,
                        "MultiRename: Select name part",
                        "Ok",
                        "This tool is only available if you have " \
                        "files in the processing list.");
      }
      break;
    }
    case GID_BTN_NAME_DATE:
    {
      insertCommandToStrGadget(pApp, GID_STR_NAME, "[YMD]");
      break;
    }
    case GID_BTN_NAME_TIME:
    {
      insertCommandToStrGadget(pApp, GID_STR_NAME, "[hms]");
      break;
    }
    case GID_BTN_NAME_COUNTER:
    {
      insertCommandToStrGadget(pApp, GID_STR_NAME, "[C]");
      break;
    }
    case GID_BTN_EXTENSION:
    {
      insertCommandToStrGadget(pApp, GID_STR_EXTENSION, "[E]");
      break;
    }
    case GID_BTN_EXTENSION_PART:
    {
      pApp->NameGadgetBufferPos = getStrGadgetBufferPos(m_ppGadgets[GID_STR_NAME]);
      pApp->ExtGadgetBufferPos = getStrGadgetBufferPos(m_ppGadgets[GID_STR_EXTENSION]);
      if((pFileNode = getLongestOldExtNode(pApp->pFiles)))
      {
        pApp->RangeMask.RequestedRangeType = RRT_EXTENSION;
        openRangeSelectWindow(pApp->pRangeSelectWindow,
                              pApp->pIntuiWindow,
                              &pApp->SigMask,
                              &pApp->RangeMask,
                              pFileNode->OriginalName 
                                + pFileNode->OriginalNameLen 
                                + 1,
                              pFileNode->OriginalExtLen);
      }
      else
      {
        showEasyRequest(pApp->pWinObject, 
                        pApp->pIntuiWindow,
                        "MultiRename: Select extension part",
                        "Ok",
                        "This tool is only available if you have " \
                        "files in the processing list and if at least " \
                        "one of them has an extension like '.iff'.");
      }
      break;
    }
    case GID_BTN_EXTENSION_COUNTER:
    {
      insertCommandToStrGadget(pApp, GID_STR_EXTENSION, "[C]");
      break;
    }
    case GID_BTN_START:
    {
      if(updateNewNames(pApp))
      {
          // Detach list from ListBrowser. Must be done because
          // `startRename()` changes the list (removes the successfully
          // renamed files)
          SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
          pApp->pIntuiWindow, 
          NULL,
          LISTBROWSER_Labels, ~0,
          TAG_DONE);
          
          startRename(pApp);
          applyNewFiles(pApp);  // Re-attach files node list to ListBrowser
                                // to display the errorous files.
      }
      break;
    }
  }
}

static void handleMenu(Application* pApp, ULONG result)
{
  struct MenuItem* pItem;
  APTR pUserData;
  ULONG selection;
  struct FileRequester* pFileReq;
  selection = (result & WMHI_MENUMASK);

  while (selection != MENUNULL && !pApp->IsExitRequested)
  {
    pItem = ItemAddress(pApp->pIntuiWindow->MenuStrip, selection);
    pUserData = GTMENUITEM_USERDATA(pItem);

    if(!pUserData)
    {
      break;
    }

    switch((ULONG)pUserData)
    {
      case MENU_PROJECT_NEW:
      {
        // Detach list from ListBrowser. Must be done before changing the list.
        SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
        pApp->pIntuiWindow, 
        NULL,
        LISTBROWSER_Labels, ~0,
        TAG_DONE);

        // Reset files and filedir lock
        freeFileNodes(pApp->pFiles);

        // Create a new, empty filenodes list
        if((pApp->pFiles = createFileNodes()))
        {
          // Attach changed list to ListBrowser.
          SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LBR_PROCESSING_LIST],
                          pApp->pIntuiWindow, 
                          NULL,
                          LISTBROWSER_Labels, (ULONG)pApp->pFiles->pList,
                          TAG_DONE);
        }
        else
        {
          PutStr("Failed to re-create the files list.\n");
          disposeApplication(pApp);
        }

        updateMainWindowTitle(pApp);
        break;
      }
      
      case MENU_PROJECT_ADD_FILES:
      {
        if((pFileReq = showMultiFileSelector(pApp->pWinObject,
                                             pApp->pIntuiWindow,
                                             "Select files to rename")))
        {
          appendFilesByWbArgs(pApp, pFileReq->fr_ArgList, pFileReq->fr_NumArgs);
          freeMultiFileSelector(pFileReq);
          pFileReq = NULL;
        }
        break;
      }

      case MENU_PROJECT_ABOUT:
      {
        showEasyRequest(pApp->pWinObject,
                        pApp->pIntuiWindow,
                        "MultiRename",
                        "Ok",
                        pApp->pAboutMessage);
        break;
      }

      case MENU_PROJECT_QUIT:
      {
        pApp->IsExitRequested = TRUE;
        break;
      }

      case MENU_SETTINGS_SKIPICONS:
      {
        pApp->pParsedArgs->AreIconsSkipped = (pItem->Flags & CHECKED);

        // Because skipping icons affects on the allowed new name length
        // (5 bytes more allowed because of the missing ".info")
        updateNewNames(pApp);
        break;
      }

      case MENU_SETTINGS_LONGNAMES:
      {
        pApp->pParsedArgs->AreLongNamesAllowed = (pItem->Flags & CHECKED);

        // Because it directly affects the allowed new name length
        updateNewNames(pApp);
        break;
      }
    }

    // Essential for processing more than one menu selection
    selection = pItem->NextSelect;
  }
}

void intuiEventLoop(Application* pApp)
{
  ULONG result;
  ULONG code;

  GetAttr(WINDOW_SigMask, pApp->pWinObject, &pApp->SigMask);

  while (!pApp->IsExitRequested)
  {
    Wait(pApp->SigMask);

    // Handle the events of the range select window
    handleRangeSelectWindowEvents(pApp->pRangeSelectWindow);

    // Handle the events of this (main) window
    while ((result = DoMethod(pApp->pWinObject, WM_HANDLEINPUT, &code)))
    {
      switch (result & WMHI_CLASSMASK)
      {
        case WMHI_CLOSEWINDOW:
          pApp->IsExitRequested = TRUE;
          break;
        case WMHI_GADGETUP:
          handleGadgets(pApp, result);
          break;
        case WMHI_MENUPICK:
          handleMenu(pApp, result);
          break;
        case WMHI_MOUSEBUTTONS:
          if(code == SELECTDOWN || code == MENUDOWN)
          {
            updateNewNames(pApp);
          }
          break;
        case WMHI_ACTIVE:
        case WMHI_INACTIVE:
          updateNewNames(pApp);
          break;
      }
    }

    // If the range select window was closed with ACCEPTED state apply
    // its result (selected range) and update the new names column.
    if(pApp->pRangeSelectWindow->WindowState == RSW_STATE_ACCEPTED)
    {
      pApp->pRangeSelectWindow->WindowState = RSW_STATE_IDLE;
      if(TRUE == applySelectedRange(pApp))
      {
        updateNewNames(pApp);
      }
      else
      {
        printf("Failed to apply selected range\n");
      }
    }
  }
}

Object* createLayout(void)
{
  Object *pMainLayout = NULL, *pTopParentHLayout = NULL, 
         *pTopVLayoutName = NULL, *pTopVLayoutExt = NULL,
         *pTopVLayoutCnt = NULL;

  m_pColumnInfo = AllocLBColumnInfo(4,
                                    LBCIA_Column, 0,
                                      LBCIA_Sortable, FALSE,
                                      LBCIA_Title, "State",
                                    LBCIA_Column, 1,
                                      LBCIA_Sortable, FALSE,
                                      LBCIA_Title, "Type",
                                    LBCIA_Column, 2,
                                      LBCIA_AutoSort, TRUE,
                                      LBCIA_SortArrow, TRUE,
                                      LBCIA_SortDirection, LBMSORT_FORWARD,
                                      LBCIA_Title, "Old name",
                                    LBCIA_Column, 3,
                                      LBCIA_Sortable, FALSE,
                                      LBCIA_Title, "New name",
                                    TAG_DONE);

  pTopVLayoutName = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Name",
    LAYOUT_AddChild, m_ppGadgets[GID_STR_NAME] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STR_NAME,
      GA_RelVerify, TRUE,
      STRINGA_TextVal, (ULONG)"[C] - [N]",
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
        GA_Text, (ULONG)"[N#-#] Part...",
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
      GA_Text, (ULONG)"[E#-#] Part...",
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
        LISTBROWSER_AutoFit, TRUE,
        LISTBROWSER_ColumnInfo, (ULONG)m_pColumnInfo,
        LISTBROWSER_ColumnTitles, TRUE,
        LISTBROWSER_HorizontalProp, TRUE,
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

///

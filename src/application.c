#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <utility/hooks.h>

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

#include "file_node.h"
#include "notifications.h"
#include "requester.h"
#include "application.h"

/**
 * Private function declarations
 */
void intuiEventLoop(Application* pApp);
Object* createLayout(void);



/**
 * Private variables
 */

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


Application* createApplication(int argc, char **argv)
{
  Object* pMainLayout;
  Application* pApp;

  if((pApp = AllocVec(sizeof(Application), MEMF_CLEAR)))
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
                                             WA_DragBar, TRUE,
                                             WA_CloseGadget, TRUE,
                                             WA_DepthGadget, TRUE,
                                             WA_SizeGadget, TRUE,
                                             WA_InnerWidth, 600,
                                             WA_InnerHeight, 400,
                                             WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_GADGETUP,
                                             WINDOW_Layout, pMainLayout,
                                             TAG_DONE)))
            {
              return pApp;
            }
            else
            {
              PutStr("Failed to create window.\n");
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

  FreeVec(pApp);
}


BOOL runApplication(Application* pApp)
{
  STRPTR pFirstPath;
  if(!pApp)
  {
    return FALSE;
  }

  // Does list contain at least one file?
  if((pFirstPath = getFirstFilePath(pApp->pFileList)))
  {
    // Display the files list in ListBrowser
    SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_LISTBROWSER],
                   NULL, NULL,
                   LISTBROWSER_Labels, (ULONG)pApp->pFileList,
                   TAG_DONE);

    // Apply the file path for this session
    strncpy(pApp->FilesPath, pFirstPath, MAXPATHLEN);
    addNotification(pApp->pNotificationsList, NNT_SELECTED_PATH_INFO, pFirstPath);
  }

  if((pApp->pIntuiWindow =
    (struct Window*)DoMethod(pApp->pWinObject, WM_OPEN, NULL)))
  {
    updateApplicationWindowTitle(pApp);

    if(containsSkippedNotifications(pApp->pNotificationsList))
    {
      sprintf(pApp->ScratchBuf,
              "Not all input files");

      if(!showEasyRequest(pApp->pIntuiWindow,
                          "Continue|Show errors",
                          "Failed to add some of the input files"))
      {
        printNotifications(pApp->pNotificationsList);
        clearNotificationsExcept(pApp->pNotificationsList,
                                 NNT_SELECTED_PATH_INFO);
      }
    }

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

void handleGadgets(Application* pApp, ULONG result)
{
  switch ((result & WMHI_GADGETMASK))
  {
  case GID_BTN_NAME:
    printf("name\n");
    break;
  }
}


void intuiEventLoop(Application* pApp)
{
  ULONG winSig;
  ULONG receivedSig;
  ULONG result;
  ULONG code;
  BOOL end = FALSE;

  GetAttr(WINDOW_SigMask, pApp->pWinObject, &winSig);

  while (!end)
  {
    receivedSig = Wait(winSig);
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

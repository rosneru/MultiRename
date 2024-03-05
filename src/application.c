#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>

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

#include "application.h"

/**
 * Private function declarations
 */
struct List* createChooserLabels(void);
void freeChooserLabels(struct List* pLabelsList);

struct List* createFilesList(UBYTE **ppLabels1, UBYTE **ppLabels2);
void freeFilesList(struct List* pFilesList);

void intuiEventLoop(Application* pApp);
Object* createLayout(struct List* pChooserLabelsList,
                     struct List* pFilesList);


/**
 * Private variables
 */
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

static Object* gadgets[MAXGADGETS];

UBYTE* dummyCol1[] =
{
  "My_1st_file_is_older_than.md",
  "my_2nd_one_and_even_more_than.txt",
  "My_3rd_attempt.doc",
  NULL
};

UBYTE* dummyCol2[] =
{
  "File-1.md",
  "File-2.txt",
  "File-3.doc",
  NULL
};

Application* createApplication(void)
{
  Object* pMainLayout;
  Application* pApp;
  struct List* pChooserList = createChooserLabels();
  struct List* pFilesList = createFilesList(dummyCol1, dummyCol2);

  if((pApp = AllocVec(sizeof(Application), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    if((pMainLayout = createLayout(pChooserList, pFilesList)))
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
                                       WA_IDCMP, IDCMP_CLOSEWINDOW,
                                       WINDOW_Layout, pMainLayout,
                                       TAG_DONE)))
      {
        return pApp;
      }
      else
      {
        PutStr("Failed to create window.\n");
        DisposeObject(pMainLayout);
        FreeVec(pApp);
      }
    }
    else
    {
      PutStr("Failed to create layout.\n");
      FreeVec(pApp);
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
  if(NULL == pApp)
  {
    return;
  }

  if(NULL != pApp->pWinObject)
  {
    DisposeObject(pApp->pWinObject);
  }

  FreeVec(pApp);
}


BOOL runApplication(Application* pApp)
{
  if(NULL == pApp)
  {
    return FALSE;
  }

  if((pApp->pIntuiWindow =
    (struct Window*)DoMethod(pApp->pWinObject, WM_OPEN, NULL)))
  {
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




void intuiEventLoop(Application* pApp)
{
  ULONG winSig;
  ULONG receivedSig;
  ULONG result;
  ULONG code;
  BOOL end = FALSE;

  GetAttr(WINDOW_SigMask, pApp->pWinObject, &winSig);

  while (FALSE == end)
  {
    receivedSig = Wait(winSig);
    while ((result = DoMethod(pApp->pWinObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
    {
      switch (result & WMHI_CLASSMASK)
      {
      case WMHI_CLOSEWINDOW:
        end = TRUE;
        break;
      }
    }
  }
}




struct List* createChooserLabels(void)
{
  ULONG i = 0;
  UBYTE *values[] = { "1", "2", "4", "5", "6", "7", "8", "9", "10", NULL };
  struct Node* pLabelNode;
  struct List* pLabelsList;

  if(NULL == (pLabelsList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pLabelsList);

  do
  {
    pLabelNode = AllocChooserNode(CNA_Text, (ULONG)values[i],
                                  TAG_DONE);
    if(NULL == pLabelNode)
    {
      freeChooserLabels(pLabelsList);
      return NULL;
    }

    AddTail(pLabelsList, pLabelNode);

    i++;
  }
  while(values[i] != NULL);

  return pLabelsList;
}

void freeChooserLabels(struct List* pLabelsList)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(NULL == pLabelsList)
  {
    return;
  }

  pWorkNode = pLabelsList->lh_Head;
  while(NULL != (pNextNode = pWorkNode->ln_Succ))
  {
    FreeChooserNode(pWorkNode);
    pWorkNode = pNextNode;
  }

  FreeVec(pLabelsList);
}

struct List* createFilesList(UBYTE **ppLabels1, UBYTE **ppLabels2)
{
  struct Node *pNode;
  struct List* pFilesList;
  ULONG i = 0;

  if(NULL == (pFilesList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pFilesList);

  while (NULL != *ppLabels1)
  {
    if(*ppLabels2 == NULL)
    {
      break;
    }

    if (NULL != (pNode = AllocListBrowserNode(2, 
                                              LBNA_Column, 0,
                                                LBNCA_CopyText, FALSE,
                                                LBNCA_Editable, FALSE,
                                                LBNCA_MaxChars, 101,
                                                LBNCA_Text, (ULONG)*ppLabels1,
                                              LBNA_Column, 1,
                                                LBNCA_CopyText, FALSE,
                                                LBNCA_Editable, TRUE,
                                                LBNCA_MaxChars, 101,
                                                LBNCA_Text, (ULONG)*ppLabels2,
                                              TAG_DONE)))
    {
      AddTail(pFilesList, pNode);
    }
    else
    {
      break;
    }

    ppLabels1++;
    ppLabels2++;
    i++;
  }

  return pFilesList;
}


void freeFilesList(struct List* pFilesList)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(NULL == pFilesList)
  {
    return;
  }

  pWorkNode = pFilesList->lh_Head;
  while(NULL != (pNextNode = pWorkNode->ln_Succ))
  {
    FreeListBrowserNode(pWorkNode);
    pWorkNode = pNextNode;
  }

  FreeVec(pFilesList);
}

struct ColumnInfo columnInfo[] =
{
  { 50, "Old name", CIF_WEIGHTED },
  { 50, "New name", CIF_WEIGHTED },
  { -1, (STRPTR)~0, -1 }
};

Object* createLayout(struct List* pChooserLabelsList,
                     struct List* pFilesList)
{
  Object *pMainLayout = NULL, *pTopParentHLayout = NULL, 
         *pTopVLayoutName = NULL, *pTopVLayoutExt = NULL,
         *pTopVLayoutCnt = NULL;


  pTopVLayoutName = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Name",
    LAYOUT_AddChild, gadgets[GID_STRING_NAME] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STRING_NAME,
      STRINGA_TextVal, (ULONG)"[N]",
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, gadgets[GID_BTN_NAME] = NewObject(BUTTON_GetClass(), NULL,
        GA_ID, GID_BTN_NAME,
        GA_Text, (ULONG)"[N] Name",
      TAG_DONE),
      LAYOUT_AddChild, gadgets[GID_BTN_NAME_DATE] = NewObject(BUTTON_GetClass(), NULL,
        GA_Text, (ULONG)"[YMD] Date",
        GA_ID, GID_BTN_NAME_DATE,
      TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, gadgets[GID_BTN_NAME_PART] = NewObject(BUTTON_GetClass(), NULL,
        GA_ID, GID_BTN_NAME_PART,
        GA_Text, (ULONG)"[N#-#] Part",
      TAG_DONE),
      LAYOUT_AddChild, gadgets[GID_BTN_NAME_TIME] = NewObject(BUTTON_GetClass(), NULL,
        GA_ID, GID_BTN_NAME_TIME,
        GA_Text, (ULONG)"[hms] Time",
      TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, gadgets[GID_BTN_NAME_COUNTER] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_NAME_COUNTER,
      GA_Text, (ULONG)"[C] Counter",
    TAG_DONE),
  TAG_DONE);

  pTopVLayoutExt = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, (ULONG)"Extension",
    LAYOUT_AddChild, gadgets[GID_STRING_EXTENSION] = NewObject(STRING_GetClass(), NULL,
      GA_ID, GID_STRING_EXTENSION,
      STRINGA_TextVal, (ULONG)"[E]",
    TAG_DONE),
    LAYOUT_AddChild, gadgets[GID_BTN_EXTENSION] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_EXTENSION,
      GA_Text, (ULONG)"[E] Ext.",
    TAG_DONE),
    LAYOUT_AddChild, gadgets[GID_BTN_EXTENSION_PART] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_EXTENSION_PART,
      GA_Text, (ULONG)"[E#-#] Part",
    TAG_DONE),
    LAYOUT_AddChild, gadgets[GID_BTN_EXTENSION_COUNTER] = NewObject(BUTTON_GetClass(), NULL,
      GA_ID, GID_BTN_EXTENSION_COUNTER,
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
      LAYOUT_AddChild, gadgets[GID_INTEGER_COUNTER_START] = NewObject(INTEGER_GetClass(), NULL,
        GA_ID, GID_INTEGER_COUNTER_START,
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
      LAYOUT_AddChild, gadgets[GID_INTEGER_COUNTER_STEP] = NewObject(INTEGER_GetClass(), NULL,
        GA_ID, GID_INTEGER_COUNTER_STEP,
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
      LAYOUT_AddChild, gadgets[GID_CHOOSER_COUNTER_PLACES] = NewObject(CHOOSER_GetClass(), NULL,
        GA_ID, GID_CHOOSER_COUNTER_PLACES,
        GA_TabCycle, TRUE,
        GA_RelVerify, TRUE,
        CHOOSER_Labels, (ULONG)pChooserLabelsList,
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
    LAYOUT_DeferLayout, TRUE, /* this tag instructs layout.gadget to
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
      LAYOUT_AddChild, gadgets[GID_LISTBROWSER] = NewObject(LISTBROWSER_GetClass(), NULL,
        GA_ID, GID_LISTBROWSER,
        GA_RelVerify, TRUE,
        GA_ReadOnly, TRUE,
        LISTBROWSER_Labels, (ULONG)pFilesList,
        LISTBROWSER_ColumnInfo, (ULONG)&columnInfo,
        LISTBROWSER_ColumnTitles, TRUE,
      TAG_DONE),
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
        LAYOUT_AddChild, gadgets[GID_BTN_START] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_START,
          GA_Text, (ULONG)"Start",
        TAG_DONE),
        CHILD_WeightedWidth, 0,
      TAG_DONE),
      CHILD_WeightedHeight, 0,
    TAG_DONE),
  TAG_DONE);

  return pMainLayout;
}

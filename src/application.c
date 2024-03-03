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


UBYTE *dummyCol1[] =
{
  "My_1st_file_is_older_than.md",
  "my_2nd_one_and_even_more_than.txt",
  "My_3rd_attempt.doc",
  NULL
};

UBYTE *dummyCol2[] =
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
  struct TagItem allocTags[2];
  allocTags[0].ti_Tag = CNA_Text;
  allocTags[1].ti_Tag = TAG_DONE;

  if(NULL == (pLabelsList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pLabelsList);

  do
  {
    allocTags[0].ti_Data = (ULONG)values[i];
    pLabelNode = AllocChooserNodeA(allocTags);
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


struct TagItem columnTags[] = { {LBNA_Column, 0},
                                  {LBNCA_CopyText, TRUE},
                                  {LBNCA_Editable, TRUE},
                                  {LBNCA_MaxChars, 101},
                                  {LBNCA_Text, NULL},
                                {LBNA_Column, 1},
                                  {LBNCA_CopyText, TRUE},
                                  {LBNCA_Editable, TRUE},
                                  {LBNCA_MaxChars, 101},
                                  {LBNCA_Text, NULL},
                                {TAG_DONE}
                              };

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

    columnTags[4].ti_Data = (ULONG)*ppLabels1;
    columnTags[9].ti_Data = (ULONG)*ppLabels2;

    if (NULL != (pNode = AllocListBrowserNodeA(2, columnTags)))
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


Object* createLayout(struct List* pChooserLabelsList,
                     struct List* pFilesList)
{
  Object *pMainLayout = NULL, *pTopParentHLayout = NULL, 
         *pTopVLayoutName = NULL, *pTopVLayoutExt = NULL,
         *pTopVLayoutCnt = NULL;

  struct ColumnInfo columnInfo[] =
  {
    { 50, "Old name", 0 },
    { 50, "New name", 0 },
    { -1, (STRPTR)~0, -1 }
  };

  pTopVLayoutName = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, "Name",
    LAYOUT_AddChild, NewObject(STRING_GetClass(), NULL,
      STRINGA_TextVal, "[N]",
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
        GA_Text, "[N] Name",
      TAG_DONE),
      LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
        GA_Text, "[YMD] Date",
      TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
        GA_Text, "[N#-#] Part",
      TAG_DONE),
      LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
        GA_Text, "[hms] Time",
      TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
      GA_Text, "[C] Counter",
    TAG_DONE),
  TAG_DONE);

  pTopVLayoutExt = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, "Extension",
    LAYOUT_AddChild, NewObject(STRING_GetClass(), NULL,
      STRINGA_TextVal, "[E]",
    TAG_DONE),
    LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
      GA_Text, "[E] Ext.",
    TAG_DONE),
    LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
      GA_Text, "[E#-#] Part",
    TAG_DONE),
    LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
      GA_Text, "[C] Counter",
    TAG_DONE),
  TAG_DONE),

  pTopVLayoutCnt = NewObject(LAYOUT_GetClass(), NULL,
    LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_Label, "Define counter",
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, NewObject(INTEGER_GetClass(), NULL,
        GA_TabCycle, TRUE,
        INTEGER_Number, 1,
        INTEGER_MaxChars, 2,
        INTEGER_Minimum, 0,
        INTEGER_Maximum, 10,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Start:", TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, NewObject(INTEGER_GetClass(), NULL,
        GA_TabCycle, TRUE,
        INTEGER_Number, 1,
        INTEGER_MaxChars, 2,
        INTEGER_Minimum, 1,
        INTEGER_Maximum, 10,
      TAG_DONE),
      LABEL_Text, "Step",
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Step:", TAG_DONE),
    TAG_DONE),
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
      LAYOUT_AddChild, NewObject(CHOOSER_GetClass(), NULL,
        GA_TabCycle, TRUE,
        GA_RelVerify, TRUE,
        CHOOSER_Labels, pChooserLabelsList,
        CHOOSER_Selected, 1,
        CHOOSER_AutoFit, TRUE,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Places:", TAG_DONE),
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
    LAYOUT_AddChild, pTopParentHLayout,
    CHILD_WeightedHeight, 0,
    LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_BevelStyle, BVS_GROUP,
      LAYOUT_Label, "Processing list",
      LAYOUT_AddChild, NewObject(LISTBROWSER_GetClass(), NULL,
        GA_RelVerify, TRUE,
        LISTBROWSER_Labels, pFilesList,
        LISTBROWSER_ColumnInfo, &columnInfo,
        LISTBROWSER_ColumnTitles, TRUE,
        LISTBROWSER_Separators, TRUE,
        LISTBROWSER_Hierarchical, TRUE,
        LISTBROWSER_Editable, TRUE,
        LISTBROWSER_MultiSelect, TRUE,
        LISTBROWSER_ShowSelected, TRUE,
      TAG_DONE),
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
        LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
          GA_Text, "Start",
        TAG_DONE),
        CHILD_WeightedWidth, 0,
      TAG_DONE),
      CHILD_WeightedHeight, 0,
    TAG_DONE),
  TAG_DONE);

  return pMainLayout;
}

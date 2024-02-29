/**
 * Compile: sc main.c LINK NOSTACKCHECK
 */
#include <stdlib.h>

#include <classes/window.h>
#include <exec/memory.h>
#include <exec/types.h>
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


/**
 * Initialize Library bases with NULL to avoid auto opening attempt by
 * gcc using the wrong library names (wrong: "window.library" instead
 * of right: "window.class" etc.)
 *
 * See: https://eab.abime.net/showpost.php?p=1490638&postcount=1285
 */
// struct IntuitionBase* IntuitionBase = NULL;
struct Library* WindowBase = NULL;
// struct Library* LayoutBase = NULL;
// struct Library* ButtonBase = NULL;
// struct Library* ChooserBase = NULL;
// struct Library* IntegerBase = NULL;
// struct Library* StringBase = NULL;
// struct Library* ListBrowserBase = NULL;

void cleanExit(Object* pWindowObject);
void processEvents(Object* pWindowObject);


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

struct List* createChooserLabels()
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

Object* createLayout(struct List* pChooserLabels)
{
  // Object *pMainLayout = NULL, *pTopParentHLayout = NULL, 
  //        *pTopVLayoutName = NULL, *pTopVLayoutExt = NULL,
  //        *pTopVLayoutCnt = NULL;

  // pTopVLayoutName = NewObject(LAYOUT_GetClass(), NULL,
  //   LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
  //   LAYOUT_SpaceOuter, TRUE,
  //   LAYOUT_BevelStyle, BVS_GROUP,
  //   LAYOUT_Label, "Name",
  //   LAYOUT_AddChild, NewObject(STRING_GetClass(), NULL,
  //     STRINGA_TextVal, "[N]",
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //     LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //     LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //       GA_Text, "[N] Name",
  //     TAG_DONE),
  //     LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //       GA_Text, "[YMD] Date",
  //     TAG_DONE),
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //     LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //     LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //       GA_Text, "[N#-#] Part",
  //     TAG_DONE),
  //     LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //       GA_Text, "[hms] Time",
  //     TAG_DONE),
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //     GA_Text, "[C] Counter",
  //   TAG_DONE),
  // TAG_DONE);

  // pTopVLayoutExt = NewObject(LAYOUT_GetClass(), NULL,
  //   LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
  //   LAYOUT_SpaceOuter, TRUE,
  //   LAYOUT_BevelStyle, BVS_GROUP,
  //   LAYOUT_Label, "Extension",
  //   LAYOUT_AddChild, NewObject(STRING_GetClass(), NULL,
  //     STRINGA_TextVal, "[E]",
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //     GA_Text, "[E] Ext.",
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //     GA_Text, "[E#-#] Part",
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //     GA_Text, "[C] Counter",
  //   TAG_DONE),
  // TAG_DONE),

  // pTopVLayoutCnt = NewObject(LAYOUT_GetClass(), NULL,
  //   LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
  //   LAYOUT_SpaceOuter, TRUE,
  //   LAYOUT_BevelStyle, BVS_GROUP,
  //   LAYOUT_Label, "Define counter",
  //   LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //     LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //     LAYOUT_AddChild, NewObject(INTEGER_GetClass(), NULL,
  //       GA_TabCycle, TRUE,
  //       INTEGER_Number, 1,
  //       INTEGER_MaxChars, 2,
  //       INTEGER_Minimum, 0,
  //       INTEGER_Maximum, 10,
  //     TAG_DONE),
  //     CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Start:", TAG_DONE),
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //     LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //     LAYOUT_AddChild, NewObject(INTEGER_GetClass(), NULL,
  //       GA_TabCycle, TRUE,
  //       INTEGER_Number, 1,
  //       INTEGER_MaxChars, 2,
  //       INTEGER_Minimum, 1,
  //       INTEGER_Maximum, 10,
  //     TAG_DONE),
  //     CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Step:", TAG_DONE),
  //   TAG_DONE),
  //   LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //     LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //     LAYOUT_AddChild, NewObject(CHOOSER_GetClass(), NULL,
  //       GA_TabCycle, TRUE,
  //       GA_RelVerify, TRUE,
  //       CHOOSER_Labels, pChooserLabels,
  //       CHOOSER_Selected, 1,
  //       CHOOSER_AutoFit, TRUE,
  //     TAG_DONE),
  //     CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Places:", TAG_DONE),
  //   TAG_DONE),
  // TAG_DONE);

  // pTopParentHLayout = NewObject(LAYOUT_GetClass(), NULL,
  //   LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //   LAYOUT_SpaceOuter, FALSE,
  //   LAYOUT_AddChild, pTopVLayoutName,
  //   CHILD_WeightedWidth, 70,
  //   LAYOUT_AddChild, pTopVLayoutExt,
  //   CHILD_WeightedWidth, 30,
  //   LAYOUT_AddChild, pTopVLayoutCnt,
  //   CHILD_WeightedHeight, 0,
  // TAG_DONE);

  // pMainLayout = NewObject(LAYOUT_GetClass(), NULL,
  //   LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
  //   ICA_TARGET, ICTARGET_IDCMP, /* TODO: Remove this?? */
  //   LAYOUT_SpaceOuter, TRUE,
  //   LAYOUT_BevelStyle, BVS_GROUP,
  //   LAYOUT_DeferLayout, TRUE, /* this tag instructs layout.gadget to
  //                               * defer GM_LAYOUT and GM_RENDER and ask
  //                               * the application to do them. This
  //                               * lessens the load on input.device
  //                               */
  //   LAYOUT_AddChild, pTopParentHLayout,
  //   CHILD_WeightedHeight, 0,
  //   LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //     LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
  //     LAYOUT_SpaceOuter, TRUE,
  //     LAYOUT_BevelStyle, BVS_GROUP,
  //     LAYOUT_Label, "Processing list",
  //     LAYOUT_AddChild, NewObject(LISTBROWSER_GetClass(), NULL,
  //       GA_RelVerify, TRUE,
  //       // LISTBROWSER_Labels, &list,
  //       // LISTBROWSER_ColumnInfo, &ci,
  //       LISTBROWSER_ColumnTitles, TRUE,
  //       LISTBROWSER_Separators, TRUE,
  //       LISTBROWSER_Hierarchical, TRUE,
  //       LISTBROWSER_Editable, TRUE,
  //       LISTBROWSER_MultiSelect, TRUE,
  //       LISTBROWSER_ShowSelected, TRUE,
  //     TAG_DONE),
  //     LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
  //       LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
  //       LAYOUT_AddChild, NewObject(BUTTON_GetClass(), NULL,
  //         GA_Text, "Start",
  //       TAG_DONE),
  //       CHILD_WeightedWidth, 0,
  //     TAG_DONE),
  //     CHILD_WeightedHeight, 0,
  //   TAG_DONE),
  // TAG_DONE);

  // return pMainLayout;
  return NULL;
}

int main(void)
{
  struct Window* pIntuiWin = NULL;
  Object* pWindowObject = NULL;
  Object* pMainLayout = NULL;
  struct List* pChooserList = createChooserLabels();

  // if  (NULL == pChooserList)
  // {
  //   cleanExit(NULL);
  // }

  // if (NULL == (IntuitionBase = (struct IntuitionBase*)
  //                               OpenLibrary("intuition.library", 47)))
  // {
  //   PutStr("Failed to load v47 intuition.library.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  if (NULL == (WindowBase = OpenLibrary("window.class", 47L)))
  {
    PutStr("Failed to load v47 window.class.\n");
    freeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

  // if (NULL == (LayoutBase = OpenLibrary("gadgets/layout.gadget", 47L)))
  // {
  //   PutStr("Failed to load v47 layout.gadget.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  // if (NULL == (ButtonBase = OpenLibrary("gadgets/button.gadget", 47L)))
  // {
  //   PutStr("Failed to load v47 button.gadget.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  // if (NULL == (ChooserBase = OpenLibrary("gadgets/chooser.gadget", 47L)))
  // {
  //   PutStr("Failed to load v47 chooser.gadget.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  // if (NULL == (IntegerBase = OpenLibrary("gadgets/integer.gadget", 47L)))
  // {
  //   PutStr("Failed to load v47 integer.gadget.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  // if (NULL == (StringBase = OpenLibrary("gadgets/string.gadget", 47L)))
  // {
  //   PutStr("Failed to load v47 string.gadget.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  // if (NULL == (ListBrowserBase = OpenLibrary("gadgets/listbrowser.gadget", 47L)))
  // {
  //   PutStr("Failed to load v47 listbrowser.gadget.\n");
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }

  // if(NULL == (pMainLayout = createLayout(pChooserList)))
  // {
  //   freeChooserLabels(pChooserList);
  //   cleanExit(NULL);
  // }
  PutStr("SUCC\n");

  if (NULL == (pWindowObject = NewObject(WINDOW_GetClass(), NULL,
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
                                         // WINDOW_Layout, pMainLayout,
                                         TAG_DONE)))
  {
    freeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

  if (NULL == (pIntuiWin = (struct Window*)DoMethod(pWindowObject, WM_OPEN, NULL)))
  {
    freeChooserLabels(pChooserList);
    cleanExit(pWindowObject);
  }

  processEvents(pWindowObject);
  DoMethod(pWindowObject, WM_CLOSE);
  cleanExit(pWindowObject);
}


void processEvents(Object* pWindowObject)
{
  ULONG winSig;
  ULONG receivedSig;
  ULONG result;
  ULONG code;
  BOOL end = FALSE;

  GetAttr(WINDOW_SigMask, pWindowObject, &winSig);

  while (FALSE == end)
  {
    receivedSig = Wait(winSig);
    while ((result = DoMethod(pWindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
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

void cleanExit(Object* pWindowObject)
{
  if (pWindowObject)
  {
    DisposeObject(pWindowObject);
  }

  // if (ListBrowserBase)
  // {
  //   CloseLibrary(ListBrowserBase);
  // }

  // if (StringBase)
  // {
  //   CloseLibrary(StringBase);
  // }

  // if (IntegerBase)
  // {
  //   CloseLibrary(IntegerBase);
  // }

  // if (ButtonBase)
  // {
  //   CloseLibrary(ButtonBase);
  // }

  // if (ChooserBase)
  // {
  //   CloseLibrary(ChooserBase);
  // }

  // if (LayoutBase)
  // {
  //   CloseLibrary(LayoutBase);
  // }

  if (WindowBase)
  {
    CloseLibrary(WindowBase);
  }

  // if (IntuitionBase)
  // {
  //   CloseLibrary((struct Library*)IntuitionBase);
  // }

  exit(0);
}

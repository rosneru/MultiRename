/**
 * Compile: sc main.c LINK NOSTACKCHECK
 */
#include <stdlib.h>

#include <classes/window.h>
#include <exec/types.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
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

#include <clib/reaction_lib_protos.h>


/**
 * Initialize Library bases with NULL to avoid auto opening attempt by
 * gcc using the wrong library names (wrong: "window.library" instead
 * of right: "window.class" etc.)
 *
 * See: https://eab.abime.net/showpost.php?p=1490638&postcount=1285
 */
struct IntuitionBase* IntuitionBase = NULL;
struct Library* WindowBase = NULL;
struct Library* LayoutBase = NULL;
/*
struct Library* LabelBase = NULL;
struct Library* IntegerBase = NULL;
*/

void cleanExit(Object* pWindowObject);
void processEvents(Object* pWindowObject);


UBYTE *chooser[] =
{
  "1",
  "2",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "10",
  NULL
};





int main(void)
{
  struct Window* pIntuiWin = NULL;
  Object* pWindowObject = NULL;
  Object* pMainLayout = NULL;
  struct List* pChooserList = ChooserLabels( "1","2","3", "4", "5", "6","7", "8", "9", "10", NULL );

  if  (NULL == pChooserList)
  {
    cleanExit(NULL);
  }

  if (NULL == (IntuitionBase = (struct IntuitionBase*)
                                OpenLibrary("intuition.library", 47)))
  {
    FreeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

  if (NULL == (WindowBase = OpenLibrary("window.class", 0L)))
  {
    FreeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

  if (NULL == (LayoutBase = OpenLibrary("gadgets/layout.gadget", 0L)))
  {
    FreeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

  pMainLayout = VGroupObject,
    ICA_TARGET, ICTARGET_IDCMP,
    LAYOUT_SpaceOuter, TRUE,
    LAYOUT_BevelStyle, BVS_GROUP,
    LAYOUT_DeferLayout, TRUE, /* this tag instructs layout.gadget to
                               * defer GM_LAYOUT and GM_RENDER and ask
                               * the application to do them. This
                               * lessens the load on input.device
                               */
    LAYOUT_AddChild, HGroupObject,
      LAYOUT_SpaceOuter, FALSE,
      /* 1st group: Name mask */
      LAYOUT_AddChild, VGroupObject,
        LAYOUT_SpaceOuter, TRUE,
        LAYOUT_BevelStyle, BVS_GROUP,
        LAYOUT_Label, "Name",

        LAYOUT_AddChild, StringObject,
          STRINGA_TextVal, "[N]",
        StringEnd,

        LAYOUT_AddChild, HGroupObject,
          LAYOUT_AddChild, ButtonObject,
            GA_Text, "[N] Name",
          ButtonEnd,

          LAYOUT_AddChild, ButtonObject,
            GA_Text, "[YMD] Date",
          ButtonEnd,
        LayoutEnd,

        LAYOUT_AddChild, HGroupObject,
          LAYOUT_AddChild, ButtonObject,
            GA_Text, "[N #-#] Part",
          ButtonEnd,
          LAYOUT_AddChild, ButtonObject,
            GA_Text, "[hms] Time",
          ButtonEnd,
        LayoutEnd,
        LAYOUT_AddChild, ButtonObject,
          GA_Text, "[C] Counter",
        ButtonEnd,
      LayoutEnd,
      CHILD_WeightedWidth, 70,

      /* 2nd group Extension mask */
      LAYOUT_AddChild, VGroupObject,
        LAYOUT_SpaceOuter, TRUE,
        LAYOUT_BevelStyle, BVS_GROUP,
        LAYOUT_Label, "Extension",
        LAYOUT_AddChild, StringObject,
          STRINGA_TextVal, "[E]",
        StringEnd,
        LAYOUT_AddChild, ButtonObject,
          GA_Text, "[E] Erw.",
        ButtonEnd,
        LAYOUT_AddChild, ButtonObject,
          GA_Text, "[N #-#] Part",
        ButtonEnd,
        LAYOUT_AddChild, ButtonObject,
          GA_Text, "[C] Counter",
        ButtonEnd,
      LayoutEnd,
      CHILD_WeightedWidth, 30,


      /* 3rd group: Counter settings */
      LAYOUT_AddChild, VGroupObject,
        LAYOUT_SpaceOuter, TRUE,
        LAYOUT_BevelStyle, BVS_GROUP,
        LAYOUT_Label, "Define counter",
        LAYOUT_AddChild, HGroupObject,
          LAYOUT_AddChild, IntegerObject,
            GA_TabCycle, TRUE,
            INTEGER_Number, 1,
            INTEGER_MaxChars, 2,
            INTEGER_Minimum, 0,
            INTEGER_Maximum, 10,
          End,
          CHILD_Label, LabelObject, LABEL_Text, "Start:", End,
        LayoutEnd,

        LAYOUT_AddChild, HGroupObject,
          LAYOUT_AddChild, IntegerObject,
            GA_TabCycle, TRUE,
            INTEGER_Number, 1,
            INTEGER_MaxChars, 2,
            INTEGER_Minimum, 0,
            INTEGER_Maximum, 10,
          End,
          CHILD_Label, LabelObject, LABEL_Text, "Step:", End,
        LayoutEnd,

        LAYOUT_AddChild, HGroupObject,
          LAYOUT_AddChild, ChooserObject,
            GA_RelVerify, TRUE,
            CHOOSER_Labels, pChooserList,
            CHOOSER_Selected, 1,
            CHOOSER_AutoFit, TRUE,
          ChooserEnd,
          CHILD_Label, LabelObject, LABEL_Text, "Places:", End,
        LayoutEnd,
      LayoutEnd,
      CHILD_WeightedWidth, 0,
    LayoutEnd,
    CHILD_WeightedHeight, 0,
    LAYOUT_AddChild, VGroupObject,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_BevelStyle, BVS_GROUP,
      LAYOUT_Label, "Processing list",
      LAYOUT_AddChild, ListBrowserObject,
        GA_RelVerify, TRUE,
        // LISTBROWSER_Labels, &list,
        // LISTBROWSER_ColumnInfo, &ci,
        LISTBROWSER_ColumnTitles, TRUE,
        LISTBROWSER_Separators, TRUE,
        LISTBROWSER_Hierarchical, TRUE,
        LISTBROWSER_Editable, TRUE,
        LISTBROWSER_MultiSelect, TRUE,
        LISTBROWSER_ShowSelected, TRUE,
      ListBrowserEnd,
    LayoutEnd,
    LAYOUT_AddChild, HGroupObject,
      LAYOUT_AddChild, ButtonObject,
        GA_Text, "Start",
      ButtonEnd,
      CHILD_WeightedWidth, 0,
    LayoutEnd,
    CHILD_WeightedHeight, 0,
  LayoutEnd;

  if (NULL == pMainLayout)
  {
    FreeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

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
                                         WINDOW_Layout, pMainLayout,
                                         TAG_DONE)))
  {
    FreeChooserLabels(pChooserList);
    cleanExit(NULL);
  }

  if (NULL == (pIntuiWin = (struct Window*)DoMethod(pWindowObject, WM_OPEN, NULL)))
  {
    FreeChooserLabels(pChooserList);
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

  if (IntuitionBase)
  {
    CloseLibrary((struct Library*)IntuitionBase);
  }

  if (WindowBase)
  {
    CloseLibrary(WindowBase);
  }

  if (LayoutBase)
  {
    CloseLibrary(LayoutBase);
  }

  exit(0);
}

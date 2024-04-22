#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <utility/hooks.h>
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


#include "range_select_requester.h"


enum gadids
{
    GID_STRING = 1
  , GID_BTN_OK
  , GID_BTN_CANCEL
  , MAXGADGETS
};

static Object* m_ppGadgets[MAXGADGETS];

static struct Window *pRangeSelectIntuiWindow = NULL;
static Object *pRangeSelectWindowObj = NULL;
static Object *pMainLayout;

static struct Requester BlockingReq;

void openRangeSelectRequester(Application* pApp)
{
  ULONG sigmask;
  
  pRangeSelectWindowObj = NewObject(WINDOW_GetClass(), NULL,
    WA_Activate, TRUE,
    WA_DragBar, TRUE,
    WA_DepthGadget, TRUE,
    WA_SizeGadget, TRUE,
    WA_Title, "MultiRename: Select name part",
    WA_Left, pApp->pIntuiWindow->LeftEdge + 50,
    WA_Top, pApp->pIntuiWindow->TopEdge + 30,
    WA_Width, 500,
    WA_Height, 180,
    WA_AutoAdjust, TRUE,
    WINDOW_GadgetHelp, TRUE,
    WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_GADGETUP,
    WINDOW_Layout, pMainLayout = NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_DeferLayout, TRUE,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceInner, TRUE,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_AddChild, m_ppGadgets[GID_STRING] = NewObject(STRING_GetClass(), NULL,
        GA_ID, GID_STRING,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        TAG_END),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "Select the characters to be inserted",
        TAG_END),
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        TAG_DONE),
      CHILD_WeightedHeight, 100,
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        LAYOUT_EvenSize, TRUE,
        LAYOUT_AddChild, NewObject(NULL, "button.gadget",
          GA_ID, GID_BTN_OK,
          GA_RelVerify, TRUE,
          GA_Text, "Ok",
          BUTTON_TextPadding, TRUE,
          GA_TabCycle, TRUE,
          TAG_END),
        CHILD_WeightedWidth, 1,
        LAYOUT_AddChild, NewObject(LABEL_GetClass(), NULL, LABEL_Text, "",
          TAG_END),
        CHILD_WeightedWidth, 100,
        LAYOUT_AddChild, NewObject(NULL, "button.gadget",
          GA_ID, GID_BTN_OK,
          GA_RelVerify, TRUE,
          GA_Text, "Close",
          BUTTON_TextPadding, TRUE,
          GA_TabCycle, TRUE,
          TAG_END),
        CHILD_WeightedWidth, 1,
        TAG_DONE),
      CHILD_WeightedHeight, 0,
      TAG_DONE),
    TAG_DONE);

  InitRequester(&BlockingReq);
  Request(&BlockingReq, pApp->pIntuiWindow);
  SetWindowPointer(pApp->pIntuiWindow, WA_BusyPointer, TRUE, TAG_DONE);

  pRangeSelectIntuiWindow = (struct Window *)DoMethod(pRangeSelectWindowObj, WM_OPEN, NULL);

  if(!pRangeSelectWindowObj)
  {
    closeRangeSelectRequester(pApp);
  }

  GetAttr(WINDOW_SigMask, pRangeSelectWindowObj, &sigmask);

  pApp->SigMask |= sigmask;
}

void closeRangeSelectRequester(Application* pApp)
{
  ULONG sigmask;
  if (pRangeSelectWindowObj)
  {

    GetAttr(WINDOW_SigMask, pRangeSelectWindowObj, &sigmask);
    pApp->SigMask &= ~sigmask;
    DisposeObject(pRangeSelectWindowObj);
    pRangeSelectWindowObj  = NULL;
    pRangeSelectIntuiWindow = NULL;

    SetWindowPointer(pApp->pIntuiWindow, TAG_DONE);
    EndRequest(&BlockingReq, pApp->pIntuiWindow);
  }
}

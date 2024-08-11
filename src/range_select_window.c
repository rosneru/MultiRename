#include <classes/window.h>
#include <exec/memory.h>
#include <gadgets/chooser.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <intuition/sghooks.h>
#include <utility/hooks.h>
#include <workbench/workbench.h>

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
  #include <clib/slider_protos.h>
  #include <clib/string_protos.h>
  #include <clib/utility_protos.h>
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
  #include <proto/slider.h>
  #include <proto/string.h>
  #include <proto/utility.h>
  #include <proto/window.h>
#endif

#include <stdio.h>
#include <string.h>

#include "range_select_window.h"


/**
 * Handle the gadget events for this window. R
 */
static void handleGadgets(RangeSelectWindow* pThis, ULONG result);

enum gadids
{
    GID_STRING = 1
  , GID_SLI_FROM
  , GID_SLI_TO
  , GID_BTN_OK
  , GID_BTN_CANCEL
  , MAXGADGETS
};

static Object* m_ppGadgets[MAXGADGETS];

static Object *pMainLayout;


RangeSelectWindow* createRangeSelectWindow(void)
{
  RangeSelectWindow* pRangeSelectWindow;
  if(!(pRangeSelectWindow = AllocVec(sizeof(RangeSelectWindow), MEMF_CLEAR)))
  {
    return NULL;
  }

  pRangeSelectWindow->pWinObject = NewObject(WINDOW_GetClass(), NULL,
    WA_Title, "MultiRename: Select name part",
    WA_Activate, TRUE,
    WA_CloseGadget, TRUE,
    WA_DepthGadget, TRUE,
    WA_DragBar, TRUE,
    WA_SizeGadget, FALSE,
    WA_Width, 500,
    WA_AutoAdjust, TRUE,
    WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_GADGETUP,
    WINDOW_GadgetHelp, TRUE,
    WINDOW_Layout, pMainLayout = NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_BevelStyle, BVS_GROUP,
      LAYOUT_DeferLayout, TRUE,   /* this tag instructs layout.gadget to
                                   * defer GM_LAYOUT and GM_RENDER and ask
                                   * the application to do them. This
                                   * lessens the load on input.device
                                   */
      LAYOUT_LabelWidth, 50,
      LAYOUT_EvenSize, TRUE,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_AddImage, NewObject(LABEL_GetClass(), NULL,
        LABEL_Text, "Select the characters to be inserted",
      TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_STRING] = NewObject(STRING_GetClass(), NULL,
        GA_ID, GID_STRING,
        GA_ReadOnly, TRUE,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        ICA_TARGET, ICTARGET_IDCMP,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"Longest name:", TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_SLI_FROM] = NewObject(SLIDER_GetClass(), NULL,
        GA_ID, GID_SLI_FROM,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        SLIDER_Orientation, SORIENT_HORIZ,
        SLIDER_Min, 1,
        SLIDER_Max, MAXNAMELEN,
        SLIDER_Level, 1,
        SLIDER_LevelFormat, "%2ld",
        SLIDER_LevelMaxLen, 3,
        SLIDER_LevelDomain, "222",
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"From:", TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_SLI_TO] = NewObject(SLIDER_GetClass(), NULL,
        GA_ID, GID_SLI_TO,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        SLIDER_Orientation, SORIENT_HORIZ,
        SLIDER_Min, 1,
        SLIDER_Max, MAXNAMELEN,
        SLIDER_LevelFormat, "%2ld",
        SLIDER_LevelMaxLen, 3,
        SLIDER_LevelDomain, "222",
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"To:", TAG_DONE),
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        LAYOUT_EvenSize, TRUE,
        LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
        LAYOUT_AddChild, m_ppGadgets[GID_BTN_OK] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_OK,
          GA_RelVerify, TRUE,
          GA_Text, (ULONG)"Ok",
          BUTTON_TextPadding, TRUE,
        TAG_DONE),
        CHILD_WeightedWidth, 1,
        LAYOUT_AddChild, NewObject(LABEL_GetClass(), NULL,
          LABEL_Text, "",
        TAG_DONE),
        CHILD_WeightedWidth, 100,
        LAYOUT_AddChild, m_ppGadgets[GID_BTN_CANCEL] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_CANCEL,
          GA_RelVerify, TRUE,
          GA_Text, (ULONG)"Cancel",
          BUTTON_TextPadding, TRUE,
        TAG_DONE),
        CHILD_WeightedWidth, 1,
      TAG_DONE),
    TAG_DONE),
    TAG_DONE);
  return pRangeSelectWindow;
}

BOOL openRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow,
                           struct Window* pParentIntuiWin,
                           ULONG* pParentSigMask,
                           STRPTR pLongestName,
                           ULONG longestNameLen)
{
  if(!pRangeSelectWindow || !pRangeSelectWindow->pWinObject || !pParentSigMask)
  {
    return FALSE;
  }

  if(pRangeSelectWindow->WindowState == RSW_STATE_IS_OPEN)
  {
    // Only allow one Range select window at a time
    return FALSE;
  }


  SetAttrs(pRangeSelectWindow->pWinObject,
           WA_Left, pParentIntuiWin->LeftEdge + 50,
           WA_Top, pParentIntuiWin->TopEdge + 30,
           TAG_DONE);


  InitRequester(&pRangeSelectWindow->BlockingReq);
  Request(&pRangeSelectWindow->BlockingReq, pParentIntuiWin);
  SetWindowPointer(pParentIntuiWin, WA_BusyPointer, TRUE, TAG_DONE);

  if(!(pRangeSelectWindow->pIntuiWindow = 
        (struct Window*) DoMethod(pRangeSelectWindow->pWinObject, WM_OPEN, NULL)))
  {
    return FALSE;
  }

  // STRINGA_Mark, (strlen(pLongestName)-1),
  strncpy(pRangeSelectWindow->NameWithoutExtension, pLongestName, longestNameLen);
  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STRING], pRangeSelectWindow->pIntuiWindow, NULL,
                 STRINGA_TextVal, (ULONG) pRangeSelectWindow->NameWithoutExtension,
                 TAG_DONE);

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_FROM], pRangeSelectWindow->pIntuiWindow, NULL,
                 SLIDER_Max, (ULONG)longestNameLen,
                 TAG_DONE);

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_TO], pRangeSelectWindow->pIntuiWindow, NULL,
                 SLIDER_Max, (ULONG)longestNameLen,
                 SLIDER_Level, (ULONG)longestNameLen,
                 TAG_DONE);

  pRangeSelectWindow->pParentSigMask = pParentSigMask;
  pRangeSelectWindow->pParentIntuiWindow = pParentIntuiWin;

  GetAttr(WINDOW_SigMask, pRangeSelectWindow->pWinObject, &pRangeSelectWindow->SigMask);

  // Attach signal mask of this range select window to parent window mask
  *(pRangeSelectWindow->pParentSigMask) |= pRangeSelectWindow->SigMask;

  pRangeSelectWindow->RangeFrom = 1;
  pRangeSelectWindow->RangeTo = longestNameLen;

  pRangeSelectWindow->WindowState = RSW_STATE_IS_OPEN;
  return TRUE;
}

void closeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow)
{
  if(!pRangeSelectWindow || !pRangeSelectWindow->pWinObject
  || !pRangeSelectWindow->pIntuiWindow || !pRangeSelectWindow->pParentIntuiWindow)
  {
    return;
  }


  // Detach signal mask of this range select window to parent window mask
  *(pRangeSelectWindow->pParentSigMask) &= ~pRangeSelectWindow->SigMask;

  DoMethod(pRangeSelectWindow->pWinObject, WM_CLOSE, NULL);
  pRangeSelectWindow->pIntuiWindow = NULL;

  SetWindowPointer(pRangeSelectWindow->pParentIntuiWindow, TAG_DONE);
  EndRequest(&pRangeSelectWindow->BlockingReq, pRangeSelectWindow->pParentIntuiWindow);

}

void freeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow)
{
  if(!pRangeSelectWindow)
  {
    return;
  }

  if(pRangeSelectWindow->pWinObject)
  {
    DisposeObject(pRangeSelectWindow->pWinObject);
    pRangeSelectWindow->pWinObject  = NULL;
  }

  FreeVec(pRangeSelectWindow);
}

void handleRangeSelectWindowEvents(RangeSelectWindow* pRangeSelectWindow)
{
  ULONG result;
  ULONG code;

  if(!pRangeSelectWindow || !pRangeSelectWindow->pWinObject 
  || !pRangeSelectWindow->pIntuiWindow)
  {
    return FALSE;
  }

  while ((result = DoMethod(pRangeSelectWindow->pWinObject , WM_HANDLEINPUT, &code)))
  {
    switch (result & WMHI_CLASSMASK)
    {
      case WMHI_CLOSEWINDOW:
      {
        closeRangeSelectWindow(pRangeSelectWindow);
        pRangeSelectWindow->WindowState = RSW_STATE_CANCELLED;
        break;
      }
      case WMHI_GADGETUP:
      {
        handleGadgets(pRangeSelectWindow, result);
        break;
      }
    }
  }
}


static void handleGadgets(RangeSelectWindow* pRangeSelectWindow, ULONG result)
{
  ULONG sliderFromLevel;
  ULONG sliderToLevel;
  switch ((result & WMHI_GADGETMASK))
  {
    case GID_SLI_FROM:
    {
      GetAttr(SLIDER_Level, m_ppGadgets[GID_SLI_FROM], &sliderFromLevel);
      GetAttr(SLIDER_Level, m_ppGadgets[GID_SLI_TO], &sliderToLevel);

      if(sliderFromLevel > sliderToLevel)
      {
        sliderFromLevel = sliderToLevel;
        SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_FROM], pRangeSelectWindow->pIntuiWindow, NULL,
                      SLIDER_Level, (ULONG) sliderFromLevel,
                      TAG_DONE);

        return;
      }

      break;
    }
    case GID_BTN_OK:
    {
      closeRangeSelectWindow(pRangeSelectWindow);
      pRangeSelectWindow->WindowState = RSW_STATE_ACCEPTED;
      break;
    }
    case GID_BTN_CANCEL:
    {
      closeRangeSelectWindow(pRangeSelectWindow);
      pRangeSelectWindow->WindowState = RSW_STATE_CANCELLED;
      break;
    }
  }
}

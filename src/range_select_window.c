#include <classes/window.h>
#include <exec/memory.h>
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
  #include <clib/bevel_protos.h>
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
  #include <proto/bevel.h>
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


/*
 * Applies fromLevel and toLevel into RangeSelectWindow data struct and 
 * displays the result string in the string gadget
 */
static void createResult(RangeSelectWindow* pRsw, ULONG fromLevel, ULONG toLevel);

/**
 * Handle the gadget events for this window. R
 */
static void handleGadgets(RangeSelectWindow* pThis, ULONG result);

enum gadids
{
    GID_STRING_INPUT = 1
  , GID_LABEL_LONGEST_ITEM
  , GID_STRING_RESULT_NAME
  , GID_STRING_RESULT_MASK
  , GID_SLI_FROM
  , GID_SLI_TO
  , GID_BTN_OK
  , GID_BTN_CANCEL
  , MAXGADGETS
};

static Object* m_ppGadgets[MAXGADGETS];
static Object *pMainLayout;
struct Hook m_SlidersHook;


/// Hook implementations

void __ASM__ __SAVE_DS__ SlidersMsgFunc(__REG__(a0, struct Hook *pHook),
                                        __REG__(a2, Object *pWindow),
                                        __REG__(a1, struct Message *pMsg))
{
  ULONG fromLevel;
  ULONG toLevel;
  RangeSelectWindow* pRsw = (RangeSelectWindow*)pHook->h_Data;

  if(!pRsw->pRangeMask)
  {
    return;
  }

  GetAttr(SLIDER_Level, m_ppGadgets[GID_SLI_FROM], &fromLevel);
  GetAttr(SLIDER_Level, m_ppGadgets[GID_SLI_TO], &toLevel);

  if(fromLevel <= toLevel)
  {
    createResult(pRsw, fromLevel, toLevel);
  }
}

/// Public function implementations

RangeSelectWindow* createRangeSelectWindow(void)
{
  RangeSelectWindow* pRsw;
  if(!(pRsw = AllocVec(sizeof(RangeSelectWindow), MEMF_CLEAR)))
  {
    return NULL;
  }

  m_SlidersHook.h_Entry = (ULONG (* )())SlidersMsgFunc;
  m_SlidersHook.h_SubEntry = NULL;
  m_SlidersHook.h_Data = pRsw;

  pRsw->pWinObject = NewObject(WINDOW_GetClass(), NULL,
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
      LAYOUT_AddChild, m_ppGadgets[GID_STRING_INPUT] = NewObject(STRING_GetClass(), NULL,
        GA_ID, GID_STRING_INPUT,
        GA_ReadOnly, TRUE,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        ICA_TARGET, ICTARGET_IDCMP,
      TAG_DONE),
      CHILD_Label, m_ppGadgets[GID_LABEL_LONGEST_ITEM] = NewObject(LABEL_GetClass(), NULL,
        GA_ID, GID_LABEL_LONGEST_ITEM, 
        LABEL_Text, (ULONG)"Longest item:",
        TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_SLI_FROM] = NewObject(SLIDER_GetClass(), NULL,
        GA_ID, GID_SLI_FROM,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        SLIDER_Orientation, SORIENT_HORIZ,
        SLIDER_Min, 1,
        SLIDER_Max, MAX_NAME_LEN,
        SLIDER_Level, 1,
        SLIDER_LevelFormat, "%2ld",
        SLIDER_LevelMaxLen, 3,
        SLIDER_LevelDomain, "222",
        SLIDER_DispHook, &m_SlidersHook,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"From:", TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_SLI_TO] = NewObject(SLIDER_GetClass(), NULL,
        GA_ID, GID_SLI_TO,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        SLIDER_Orientation, SORIENT_HORIZ,
        SLIDER_Min, 1,
        SLIDER_Max, MAX_NAME_LEN,
        SLIDER_LevelFormat, "%2ld",
        SLIDER_LevelMaxLen, 3,
        SLIDER_LevelDomain, "222",
        SLIDER_DispHook, &m_SlidersHook,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"To:", TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_STRING_RESULT_NAME] = NewObject(STRING_GetClass(), NULL,
        GA_ID, GID_STRING_RESULT_NAME,
        GA_ReadOnly, TRUE,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"Selection result:", TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_STRING_RESULT_MASK] = NewObject(STRING_GetClass(), NULL,
        GA_ID, GID_STRING_RESULT_MASK,
        GA_ReadOnly, TRUE,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
      TAG_DONE),
      CHILD_Label, NewObject(LABEL_GetClass(), NULL, LABEL_Text, (ULONG)"Result mask:", TAG_DONE),
      LAYOUT_AddChild, NewObject(LAYOUT_GetClass(), NULL,
        LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
        LAYOUT_EvenSize, TRUE,
        LAYOUT_AddChild, m_ppGadgets[GID_BTN_OK] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_OK,
          GA_RelVerify, TRUE,
          GA_Text, (ULONG)"Ok",
          BUTTON_TextPadding, TRUE,
        TAG_DONE),
        CHILD_WeightedWidth, 0,
        LAYOUT_AddChild, m_ppGadgets[GID_BTN_CANCEL] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_CANCEL,
          GA_RelVerify, TRUE,
          GA_Text, (ULONG)"Cancel",
          BUTTON_TextPadding, TRUE,
        TAG_DONE),
        CHILD_WeightedWidth, 0,
      TAG_DONE),
    TAG_DONE),
    TAG_DONE);
  return pRsw;
}

STRPTR pWindowTitleSelectName = "MultiRename: Select name part";
STRPTR pWindowTitleSelectExtension = "MultiRename: Select extension part";

BOOL openRangeSelectWindow(RangeSelectWindow* pRsw,
                           struct Window* pParentIntuiWin,
                           ULONG* pParentSigMask,
                           RangeMask* pRangeMask,
                           STRPTR pLongestName,
                           ULONG longestNameLen)
{
  STRPTR pWindowTitle;

  if(!pRsw || !pRsw->pWinObject || !pParentSigMask || !pRangeMask)
  {
    return FALSE;
  }

  if(pRsw->WindowState == RSW_STATE_IS_OPEN)
  {
    // Only allow one range select window at a time
    return FALSE;
  }


  SetAttrs(pRsw->pWinObject,
           WA_Left, pParentIntuiWin->LeftEdge + 50,
           WA_Top, pParentIntuiWin->TopEdge + 30,
           TAG_DONE);

  switch(pRangeMask->RequestedRangeType)
  {
    case RRT_NAME:
      pWindowTitle = pWindowTitleSelectName;

      // Create a copy of the input string 'pLongestName' with no extension
      strncpy(pRsw->NameBuf, pLongestName, longestNameLen);
      break;
    case RRT_EXTENSION:
      pWindowTitle = pWindowTitleSelectExtension;

      // Create a copy of the input string 'pLongestName' with no extension
      strncpy(pRsw->NameBuf, pLongestName, longestNameLen);
      break;
    default:
      return FALSE;
  }

  InitRequester(&pRsw->BlockingReq);
  Request(&pRsw->BlockingReq, pParentIntuiWin);
  SetWindowPointer(pParentIntuiWin, WA_BusyPointer, TRUE, TAG_DONE);

  if(!(pRsw->pIntuiWindow = 
        (struct Window*) DoMethod(pRsw->pWinObject, WM_OPEN, NULL)))
  {
    return FALSE;
  }

  SetWindowTitles(pRsw->pIntuiWindow, pWindowTitle, (UBYTE *)~0);

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STRING_INPUT], pRsw->pIntuiWindow, NULL,
                 STRINGA_TextVal, (ULONG) pRsw->NameBuf,
                 TAG_DONE);

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_FROM], pRsw->pIntuiWindow, NULL,
                 SLIDER_Min, 1,
                 SLIDER_Max, (ULONG)longestNameLen,
                 SLIDER_Level, 1,
                 TAG_DONE);

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_TO], pRsw->pIntuiWindow, NULL,
                 SLIDER_Min, 1,
                 SLIDER_Max, (ULONG)longestNameLen,
                 SLIDER_Level, (ULONG)longestNameLen,
                 TAG_DONE);

  pRsw->pRangeMask = pRangeMask;
  pRsw->pParentSigMask = pParentSigMask;
  pRsw->pParentIntuiWindow = pParentIntuiWin;

  GetAttr(WINDOW_SigMask, pRsw->pWinObject, &pRsw->SigMask);

  // Attach signal mask of this range select window to parent window mask
  *(pRsw->pParentSigMask) |= pRsw->SigMask;

  createResult(pRsw, 1, longestNameLen);

  pRsw->WindowState = RSW_STATE_IS_OPEN;
  return TRUE;
}

void closeRangeSelectWindow(RangeSelectWindow* pRsw)
{
  if(!pRsw || !pRsw->pWinObject
  || !pRsw->pIntuiWindow || !pRsw->pParentIntuiWindow)
  {
    return;
  }


  // Detach signal mask of this range select window to parent window mask
  *(pRsw->pParentSigMask) &= ~pRsw->SigMask;

  DoMethod(pRsw->pWinObject, WM_CLOSE, NULL);
  pRsw->pIntuiWindow = NULL;

  SetWindowPointer(pRsw->pParentIntuiWindow, TAG_DONE);
  EndRequest(&pRsw->BlockingReq, pRsw->pParentIntuiWindow);

}

void freeRangeSelectWindow(RangeSelectWindow* pRsw)
{
  if(!pRsw)
  {
    return;
  }

  if(pRsw->pWinObject)
  {
    DisposeObject(pRsw->pWinObject);
    pRsw->pWinObject  = NULL;
  }

  FreeVec(pRsw);
}


static void createResult(RangeSelectWindow* pRsw, ULONG fromLevel, ULONG toLevel)
{
  char commandPartBuf[MAX_RANGE_STRING_LEN + 1];
  STRPTR pInputText;
  ULONG resultLength;
  if(pRsw == NULL)
  {
    return;
  }

  pRsw->pRangeMask->RangeFrom = fromLevel;
  pRsw->pRangeMask->RangeTo = toLevel;

  resultLength = toLevel - fromLevel + 1;
  if(resultLength > MAX_NAME_LEN)
  {
    resultLength = MAX_NAME_LEN;
  }

  GetAttr(STRINGA_TextVal, m_ppGadgets[GID_STRING_INPUT], (ULONG*)&pInputText);
  strncpy(pRsw->NameBuf, pInputText + fromLevel - 1, resultLength);
  pRsw->NameBuf[resultLength] = '\0';

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STRING_RESULT_NAME], pRsw->pIntuiWindow, NULL,
                 STRINGA_TextVal, (ULONG) pRsw->NameBuf,
                 TAG_DONE);

  createRangeMaskString(pRsw->pRangeMask, commandPartBuf);

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STRING_RESULT_MASK], pRsw->pIntuiWindow, NULL,
                 STRINGA_TextVal, (ULONG) commandPartBuf,
                 TAG_DONE);
}


void handleRangeSelectWindowEvents(RangeSelectWindow* pRsw)
{
  ULONG result;
  ULONG code;

  if(!pRsw || !pRsw->pWinObject 
  || !pRsw->pIntuiWindow)
  {
    return;
  }

  while ((result = DoMethod(pRsw->pWinObject , WM_HANDLEINPUT, &code)))
  {
    switch (result & WMHI_CLASSMASK)
    {
      case WMHI_CLOSEWINDOW:
      {
        closeRangeSelectWindow(pRsw);
        pRsw->WindowState = RSW_STATE_CANCELLED;
        break;
      }
      case WMHI_GADGETUP:
      {
        handleGadgets(pRsw, result);
        break;
      }
    }
  }
}

static void handleGadgets(RangeSelectWindow* pRsw, ULONG result)
{
  ULONG fromLevel;
  ULONG toLevel;

  GetAttr(SLIDER_Level, m_ppGadgets[GID_SLI_FROM], &fromLevel);
  GetAttr(SLIDER_Level, m_ppGadgets[GID_SLI_TO], &toLevel);

  switch ((result & WMHI_GADGETMASK))
  {
    case GID_SLI_FROM:
    {
      if(fromLevel > toLevel)
      {
        fromLevel = toLevel;
        SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_FROM], pRsw->pIntuiWindow, NULL,
                       SLIDER_Level, (ULONG) fromLevel,
                       TAG_DONE);
      }

      createResult(pRsw, fromLevel, toLevel);
      break;
    }
    case GID_SLI_TO:
    {
      if(toLevel < fromLevel)
      {
        toLevel = fromLevel;
        SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_SLI_TO], pRsw->pIntuiWindow, NULL,
                       SLIDER_Level, (ULONG) toLevel,
                       TAG_DONE);
      }

      createResult(pRsw, fromLevel, toLevel);
      break;
    }
    case GID_BTN_OK:
    {
      pRsw->WindowState = RSW_STATE_ACCEPTED;
      closeRangeSelectWindow(pRsw);
      break;
    }
    case GID_BTN_CANCEL:
    {
      pRsw->WindowState = RSW_STATE_CANCELLED;
      closeRangeSelectWindow(pRsw);
      break;
    }


  }
}

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

#include <stdio.h>
#include <string.h>

#include "range_select_window.h"


/**
 * Handle the gadget events for this window. Returns FALSE in normal
 * operation and TRUE if the Ok/Apply button was pressed and the window
 * is due to be closed with a positive result.
 */
static BOOL handleGadgets(RangeSelectWindow* pThis, ULONG result);

enum gadids
{
    GID_STRING = 1
  , GID_BTN_OK
  , GID_BTN_CLOSE
  , MAXGADGETS
};

static Object* m_ppGadgets[MAXGADGETS];

static Object *pMainLayout;


// struct Hook m_EditHook;

// ULONG __ASM__ __SAVE_DS__ StringEditFunc(__REG__(a0, struct Hook *pHook),
//                                          __REG__(a2, struct SGWork * pSgWork),
//                                          __REG__(a1, ULONG *pMsg))
// {
//   RangeSelectWindow* pRsw = (RangeSelectWindow*)pHook->h_Data;
  
//   // TODO if(*pMsg == SGH_CLICK)
//   if(!GetAttr(STRINGA_Mark, m_ppGadgets[GID_STRING],  &pRsw->Marked))
//   {
//     pRsw->Marked = 666;
//   }

//   // TODO from sghooks.h:
//   //
//   // You return 0 if you don't understand the command (SGH_KEY is
//   // required and assumed).  Return non-zero if you implement the
//   // command.
//   //
//   // You should always leave the SGA_REDISPLAY flag set, since Intuition
//   // uses this processing when activating a string gadget.

//   // pSgWork->Actions |= SGA_REDISPLAY;
//   // return (~0L);
//   return 0;
// }


RangeSelectWindow* createRangeSelectWindow(void)
{
  RangeSelectWindow* pRangeSelectWindow;
  if(!(pRangeSelectWindow = AllocVec(sizeof(RangeSelectWindow), MEMF_CLEAR)))
  {
    return NULL;
  }

  // m_EditHook.h_Entry = (ULONG (*)()) StringEditFunc;
  // m_EditHook.h_SubEntry = NULL;
  // m_EditHook.h_Data = pRangeSelectWindow;

  pRangeSelectWindow->pWinObject = NewObject(WINDOW_GetClass(), NULL,
    WA_Title, "MultiRename: Select name part",
    WA_Activate, TRUE,
    WA_CloseGadget, TRUE,
    WA_DepthGadget, TRUE,
    WA_DragBar, TRUE,
    WA_SizeGadget, TRUE,
    WA_Width, 500,
    WA_AutoAdjust, TRUE,
    WINDOW_GadgetHelp, TRUE,
    WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_GADGETUP,
    WINDOW_Layout, pMainLayout = NewObject(LAYOUT_GetClass(), NULL,
      LAYOUT_EvenSize, TRUE,
      LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_BevelStyle, BVS_GROUP,
      LAYOUT_DeferLayout, TRUE,   /* this tag instructs layout.gadget to
                                   * defer GM_LAYOUT and GM_RENDER and ask
                                   * the application to do them. This
                                   * lessens the load on input.device
                                   */
      LAYOUT_AddImage, NewObject(LABEL_GetClass(), NULL,
        LABEL_Text, "Select the characters to be inserted",
      TAG_DONE),
      LAYOUT_AddChild, m_ppGadgets[GID_STRING] = NewObject(STRING_GetClass(), NULL,
        GA_ID, GID_STRING,
        GA_RelVerify, TRUE,
        GA_TabCycle, TRUE,
        // STRINGA_EditHook, &m_EditHook,
        ICA_TARGET, ICTARGET_IDCMP,
      TAG_DONE),
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
        LAYOUT_AddChild, m_ppGadgets[GID_BTN_CLOSE] = NewObject(BUTTON_GetClass(), NULL,
          GA_ID, GID_BTN_CLOSE,
          GA_RelVerify, TRUE,
          GA_Text, (ULONG)"Close",
          BUTTON_TextPadding, TRUE,
        TAG_DONE),
        CHILD_WeightedWidth, 1,
      TAG_DONE),
      CHILD_WeightedHeight, 0,
    TAG_DONE),
    TAG_DONE);
  return pRangeSelectWindow;
}

BOOL openRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow,
                           struct Window* pParentIntuiWin,
                           ULONG* pParentSigMask,
                           STRPTR pStringGadgetText)
{
  if(!pRangeSelectWindow || !pRangeSelectWindow->pWinObject || !pParentSigMask)
  {
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

  SetGadgetAttrs((struct Gadget *) m_ppGadgets[GID_STRING], pRangeSelectWindow->pIntuiWindow, NULL,
                 STRINGA_TextVal, (ULONG) pStringGadgetText,
                 STRINGA_Mark, (strlen(pStringGadgetText)-1),
                 TAG_DONE);

  ActivateLayoutGadget((struct Gadget*)pMainLayout,
                       pRangeSelectWindow->pIntuiWindow,
                       NULL,
                       (ULONG)m_ppGadgets[GID_STRING]);

  pRangeSelectWindow->pParentSigMask = pParentSigMask;
  pRangeSelectWindow->pParentIntuiWindow = pParentIntuiWin;

  GetAttr(WINDOW_SigMask, pRangeSelectWindow->pWinObject, &pRangeSelectWindow->SigMask);

  // Attach signal mask of this range select window to parent window mask
  *(pRangeSelectWindow->pParentSigMask) |= pRangeSelectWindow->SigMask;

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

BOOL handleRangeSelectWindowEvents(RangeSelectWindow* pRangeSelectWindow)
{
  ULONG result;
  ULONG code;
  BOOL isWindowClosedWithOk = FALSE;

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
        break;
      }
      case WMHI_GADGETUP:
      {
        isWindowClosedWithOk = handleGadgets(pRangeSelectWindow, result);
        break;
      }
      case IDCMP_IDCMPUPDATE:
      {
        // Only the StringGadget sends these messages for now. No need 
        // to
printf("IDCMP_IDCMPUPDATE\n");
        if(!GetAttr(STRINGA_Mark,
                    m_ppGadgets[GID_STRING],
                    &pRangeSelectWindow->Marked))
        {
          pRangeSelectWindow->Marked = 666;
        }
        break;
      }
    }
  }

  return isWindowClosedWithOk;
}


static BOOL handleGadgets(RangeSelectWindow* pRangeSelectWindow, ULONG result)
{
  switch ((result & WMHI_GADGETMASK))
  {
    case GID_STRING:
    {
    }
    case GID_BTN_OK:
    {
      printf("Marked = %d\n", pRangeSelectWindow->Marked);
      closeRangeSelectWindow(pRangeSelectWindow);
      return TRUE;
      break;
    }
    case GID_BTN_CLOSE:
    {
      closeRangeSelectWindow(pRangeSelectWindow);
      break;
    }
  }

  return FALSE;
}

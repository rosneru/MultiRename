#ifndef RANGE_SELECT_WINDOW_H
#define RANGE_SELECT_WINDOW_H

typedef struct RangeSelectWindow
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  struct Window* pParentIntuiWindow;
  ULONG* pMainSigMask;
  struct Requester BlockingReq;
  UBYTE RangeFrom;
  UBYTE RangeTo;
} RangeSelectWindow;

RangeSelectWindow* createRangeSelectWindow();
BOOL openRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow,
                           struct Window* pParentIntuiWin,
                           ULONG* pMainSigMask,
                           STRPTR pStringGadgetText);
void closeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);
void freeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);

/**
 * Handles the Intuition events for this window. Returns TRUE if as a
 * result of this the window was closed positively with the Ok/Apply
 * button. If the window was closed with its close gadget or the cancel
 * button or if it was not closed at all during normal operation, it
 * returns FALSE.
 */
BOOL handleRangeSelectWindowEvents(RangeSelectWindow* pRangeSelectWindow);

#endif

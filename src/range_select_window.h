#ifndef RANGE_SELECT_WINDOW_H
#define RANGE_SELECT_WINDOW_H

typedef struct RangeSelectWindow
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  struct Window* pParentIntuiWindow;
  ULONG* pMainSigMask;
  struct Requester BlockingReq;
} RangeSelectWindow;

RangeSelectWindow* createRangeSelectWindow();
BOOL openRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow,
                           struct Window* pParentIntuiWin,
                           ULONG* pMainSigMask);
void closeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);
void freeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);

void handleRangeSelectWindowEvents(RangeSelectWindow* pRangeSelectWindow);

#endif

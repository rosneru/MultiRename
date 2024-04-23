#ifndef RANGE_SELECT_REQUESTER_H
#define RANGE_SELECT_REQUESTER_H

#include "application.h"

typedef struct RangeSelectData
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  struct Window* pParentIntuiWindow;
  ULONG* pMainSigMask;
  struct Requester BlockingReq;
} RangeSelectData;

RangeSelectData* createRangeSelectData();
BOOL openRangeSelectWindow(RangeSelectData* pRangeSelectData,
                           struct Window* pParentIntuiWin,
                           ULONG* pMainSigMask);
void closeRangeSelectWindow(RangeSelectData* pRangeSelectData);
void freeRangeSelectData(RangeSelectData* pRangeSelectData);

void handleRangeSelectWindowEvents(RangeSelectData* pRangeSelectData);

#endif

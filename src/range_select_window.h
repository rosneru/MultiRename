#ifndef RANGE_SELECT_WINDOW_H
#define RANGE_SELECT_WINDOW_H

#include "file_node.h"  // MAXNAMELEN
#include "range_mask.h"

typedef enum
{
    RSW_STATE_IDLE = 0
  , RSW_STATE_CANCELLED
  , RSW_STATE_ACCEPTED
  , RSW_STATE_IS_OPEN
} RangeSelectWindowState;

typedef struct RangeSelectWindow
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  ULONG SigMask;
  struct Window* pParentIntuiWindow;
  ULONG* pParentSigMask;
  struct Requester BlockingReq;
  RangeSelectWindowState WindowState;
  RangeMask* pRangeMask;
  unsigned char NameBuf[MAX_NAME_LEN + 1];
} RangeSelectWindow;

RangeSelectWindow* createRangeSelectWindow(void);
BOOL openRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow,
                           struct Window* pParentIntuiWin,
                           ULONG* pParentSigMask,
                           RangeMask* pRangeMask,
                           STRPTR pLongestName,
                           ULONG longestNameLen);
void closeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);
void freeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);

void handleRangeSelectWindowEvents(RangeSelectWindow* pRangeSelectWindow);

#endif

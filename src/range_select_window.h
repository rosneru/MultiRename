#ifndef RANGE_SELECT_WINDOW_H
#define RANGE_SELECT_WINDOW_H

#include "file_node.h"  // MAXNAMELEN

typedef enum
{
    RSW_STATE_CANCELLED = 1
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
  ULONG RangeFrom;
  ULONG RangeTo;
  unsigned char NameWithoutExtension[MAXNAMELEN + 1];
} RangeSelectWindow;

RangeSelectWindow* createRangeSelectWindow(void);
BOOL openRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow,
                           struct Window* pParentIntuiWin,
                           ULONG* pParentSigMask,
                           STRPTR pLongestName,
                           ULONG longestNameLen);
void closeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);
void freeRangeSelectWindow(RangeSelectWindow* pRangeSelectWindow);

void handleRangeSelectWindowEvents(RangeSelectWindow* pRangeSelectWindow);

#endif

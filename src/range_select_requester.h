#ifndef RANGE_SELECT_REQUESTER_H
#define RANGE_SELECT_REQUESTER_H

#include "application.h"

void openRangeSelectRequester(Application* pApp);
void closeRangeSelectRequester(Application* pApp);

void handleRangeSelectRequesterEvents(Application* pApp);

#endif

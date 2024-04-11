#ifndef RENAME_COUNTER_H
#define RENAME_COUNTER_H

#include <exec/types.h>

#define COUNTER_MAX_WIDTH 10

typedef struct Counter
{
  LONG Inc;
  BYTE Width;
  LONG Value;
  LONG ValueWhenLastRequested;
  char ValueAsStr[COUNTER_MAX_WIDTH + 1];
} Counter;


void initCounter(Counter* pCounter, LONG start, LONG inc, BYTE width);
void incrementCounter(Counter* pCounter);
STRPTR getCounterValue(Counter* pCounter);

#endif

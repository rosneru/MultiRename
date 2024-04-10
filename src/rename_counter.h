#ifndef RENAME_COUNTER_H
#define RENAME_COUNTER_H

#include <exec/types.h>

#define COUNTER_MAX_WIDTH 10
#define COUNTER_MAX_WIDTH_STR "10"

typedef struct Counter
{
  LONG Start;
  LONG Inc;
  BYTE Width;
  LONG Value;
  LONG ValueWhenLastRequested;
  char StrValue[COUNTER_MAX_WIDTH + 1];
} Counter;


void initCounter(Counter* pCounter, LONG start, LONG inc, BYTE width);
void incrementCounter(Counter* pCounter);
STRPTR getCounterValue(Counter* pCounter);

#endif

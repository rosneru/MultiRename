#include "rename_counter.h"
#include <stdio.h>

#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif

void initCounter(Counter *pCounter, LONG start, LONG inc, BYTE width)
{
  if (pCounter == NULL)
  {
    return;
  }

  pCounter->Value = start;
  if (pCounter->Value < 0)
  {
    pCounter->Value = 0;
  }

  pCounter->Inc = inc;
  if (pCounter->Inc < 1)
  {
    pCounter->Inc = 1;
  }

  pCounter->Width = width;
  if (pCounter->Width > COUNTER_MAX_WIDTH)
  {
    pCounter->Width = COUNTER_MAX_WIDTH;
  }

  pCounter->Value = start;
  pCounter->ValueWhenLastRequested = -1;
}

void incrementCounter(Counter *pCounter)
{
  if (pCounter == NULL)
  {
    return;
  }

  if ((LONG_MAX - pCounter->Value) > pCounter->Inc)
  {
    pCounter->Value += pCounter->Inc;
  }
  else
  {
    // Overflow: reset to 0
    pCounter->Value = pCounter->Inc - (LONG_MAX - pCounter->Value) - 1;
  }
}

STRPTR getCounterValue(Counter *pCounter)
{
  if (pCounter == NULL)
  {
    return 0;
  }

  if (pCounter->Value == pCounter->ValueWhenLastRequested)
  {
    return pCounter->ValueAsStr;
  }

  pCounter->ValueWhenLastRequested = pCounter->Value;
  sprintf(pCounter->ValueAsStr, "%0*d", pCounter->Width, pCounter->Value);

  return pCounter->ValueAsStr;
}

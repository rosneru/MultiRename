#include <stdio.h>
#include "rename_counter.h"


void initCounter(Counter* pCounter, LONG start, LONG inc, BYTE width)
{
  if(pCounter == NULL)
  {
    return;
  }

  pCounter->Start = start;
  if(pCounter->Start < 0)
  {
    pCounter->Start = 1;
  }


  pCounter->Inc = inc;
  if(pCounter->Inc < 1)
  {
    pCounter->Inc = 1;
  }

  pCounter->Width = width;
  if(pCounter->Width > COUNTER_MAX_WIDTH)
  {
    pCounter->Width = COUNTER_MAX_WIDTH;
  }

  pCounter->Value = start;
  pCounter->ValueWhenLastRequested = -1;
}

void  incrementCounter(Counter* pCounter)
{
  if(pCounter == NULL)
  {
    return;
  }

  pCounter->Value += pCounter->Inc;
}

STRPTR getCounterValue(Counter* pCounter)
{
  if(pCounter == NULL)
  {
    return 0;
  }

  if(pCounter->Value == pCounter->ValueWhenLastRequested)
  {
    return pCounter->StrValue;
  }

  pCounter->ValueWhenLastRequested = pCounter->Value;
  sprintf(pCounter->StrValue, "%0*d", pCounter->Width, pCounter->Value);

  return pCounter->StrValue;
}

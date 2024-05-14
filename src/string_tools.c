#include <string.h>
#include "string_tools.h"


int appendString(STRPTR pDest,
                 ULONG destSize,
                 ULONG* pNewDestLen,
                 STRPTR pSrc,
                 ULONG numChars)
{
  ULONG remainingDestSize, srcLen, currentDestLen;

  currentDestLen = strlen(pDest);
  if(currentDestLen >= (destSize - 1))
  {
    if(pNewDestLen)
    {
      *pNewDestLen = currentDestLen;
    }

    return TRUE;
  }

  remainingDestSize = destSize - currentDestLen - 1;

  srcLen = strlen(pSrc);
  if(numChars > 0)
  {
    /* TODO Is here a min/max needed? */
    srcLen = numChars;
  }

  if(srcLen > remainingDestSize)
  {
    memcpy(pDest + currentDestLen, pSrc, remainingDestSize);
    pDest[destSize - 1] = '\0';
    
    if(pNewDestLen)
    {
      *pNewDestLen = currentDestLen + remainingDestSize;
    }

    return TRUE;
  }
  else
  {
    memcpy(pDest + currentDestLen, pSrc, srcLen);
    pDest[currentDestLen + srcLen] = '\0';

    if(pNewDestLen)
    {
      *pNewDestLen = currentDestLen + srcLen;
    }

    return FALSE;
  }
}
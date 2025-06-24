#include <string.h>
#include "string_tools.h"

ULONG createStringToken(const char* pStr, ULONG strLength)
{
  ULONG i, token = 0;
  const char *pItemText;

  if(!pStr || strLength == 0)
  {
    return 0;
  }

  for (i = 0; i < strLength; i++)
  {
    token += 2 * token + *(pItemText++);
  }

  return token;
}

BOOL appendString(STRPTR pDest, ULONG destSize, ULONG* pNewDestLen, STRPTR pSrc, ULONG numChars)
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

int insertString(const STRPTR pSrcStr,
                 STRPTR pStrToInsert,
                 ULONG insertPos,
                 STRPTR pDestBuf,
                 ULONG destBufSize)
{
  if(!pStrToInsert || !pDestBuf || !pSrcStr)
  {
    return -1;
  }

  // Start with a clean target buffer
  strcpy(pDestBuf, "");

  // Apply the beginning until the insert position
  strncat(pDestBuf, pSrcStr, insertPos);
  pDestBuf[insertPos] = '\0';

  // Apply the 'string to insert'
  strcat(pDestBuf, pStrToInsert);

  // Apply the end, after the insert position
  strcat(pDestBuf, pSrcStr + insertPos);

  return (int)(insertPos + strlen(pStrToInsert));
}

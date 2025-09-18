#include <stdio.h>
#include <string.h>

#include "file_node.h" // MAXNAMELEN, MAX_CMD_PART_LEN
#include "range_mask.h"

BOOL createRangeMaskString(RangeMask *pRangeMask, char *pDestBuf)
{
  char insertCmd;
  if (!pRangeMask || !pDestBuf)
  {
    return FALSE;
  }

  switch (pRangeMask->RequestedRangeType)
  {
  case RRT_NAME:
    insertCmd = 'N';
    break;
  case RRT_EXTENSION:
    insertCmd = 'E';
    break;
  default:
    return FALSE;
  }

  sprintf(pDestBuf,
    "[%c%d-%d]",
    insertCmd,
    pRangeMask->RangeFrom,
    pRangeMask->RangeTo);

  return TRUE;
}

int insertRangeMaskString(RangeMask *pRangeMask,
  STRPTR pDestBuf,
  ULONG destBufSize,
  const STRPTR pSrcStr,
  UBYTE insertPos)
{
  char commandPartBuf[MAX_RANGE_STRING_LEN + 1];

  if (!pRangeMask || !pDestBuf || !pSrcStr
    || (pRangeMask->RangeFrom > MAX_NAME_LEN)
    || (pRangeMask->RangeTo > MAX_NAME_LEN)
    || (pRangeMask->RangeFrom > pRangeMask->RangeTo))
  {
    return -1;
  }

  if ((strlen(pSrcStr) + MAX_RANGE_STRING_LEN) > destBufSize)
  {
    return -1;
  }

  // Start with a clean target buffer
  strcpy(pDestBuf, "");

  // Apply the beginning until the insert position
  strncat(pDestBuf, pSrcStr, insertPos);
  pDestBuf[insertPos] = '\0';

  // Fill the command buf
  createRangeMaskString(pRangeMask, commandPartBuf);

  // Apply the command buf
  strcat(pDestBuf, commandPartBuf);

  // Apply the end, after the insert position
  strcat(pDestBuf, pSrcStr + insertPos);

  return (int)(insertPos + strlen(commandPartBuf));
}

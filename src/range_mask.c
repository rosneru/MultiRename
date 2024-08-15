#include <stdio.h>
#include <string.h>

#include "file_node.h"  // MAXNAMELEN
#include "range_mask.h"


#define MAX_CMD_PART_LEN 12

int insertRangedPart(RangeMask* pRangeMask,
                     STRPTR pDest,
                     STRPTR pSrcStr,
                     UBYTE insertPos)
{
  char commandPartBuf[12];
  char insertCmd;

  if(!pRangeMask || !pDest || ! pSrcStr || (insertPos < 0)
  || (pRangeMask->RangeFrom > MAXNAMELEN) || (pRangeMask->RangeTo > MAXNAMELEN) 
  || (pRangeMask->RangeFrom > pRangeMask->RangeTo))
  {
    return -1;
  }

  if((strlen(pSrcStr) + MAX_CMD_PART_LEN) > MAXNAMELEN)
  {
    return -1;
  }

  switch(pRangeMask->RequestedRangeType)
  {
    case RRT_NAME:
      insertCmd = 'N';
      break;
    case RRT_EXTENSION:
      insertCmd = 'E';
      break;
    default:
      return -1;
  }

  // Start with a clean target buffer
  strcpy(pDest, "");

  // Apply the beginning until the insert position
  strncat(pDest, pSrcStr, insertPos);
  pDest[insertPos] = '\0';

  // Fill the command buf
  sprintf(commandPartBuf, "[%c%d-%d]", insertCmd,
                                       pRangeMask->RangeFrom,
                                       pRangeMask->RangeTo);

  // Apply the command buf
  strcat(pDest, commandPartBuf);
  
  // Apply the end, after the insert position
  strcat(pDest, pSrcStr + insertPos);

  return (int)(insertPos + strlen(commandPartBuf));
}

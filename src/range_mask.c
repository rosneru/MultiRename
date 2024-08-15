#include <stdio.h>
#include <string.h>

#include "file_node.h"  // MAXNAMELEN, MAX_CMD_PART_LEN
#include "range_mask.h"


void createCommandMask(RangeMask* pRangeMask, char* pDestinationCmdBuf)
{
  char insertCmd;

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

  sprintf(pDestinationCmdBuf, "[%c%d-%d]", insertCmd,
                                           pRangeMask->RangeFrom,
                                           pRangeMask->RangeTo);
}

int insertRangedPart(RangeMask* pRangeMask,
                     STRPTR pDest,
                     STRPTR pSrcStr,
                     UBYTE insertPos)
{
  char commandPartBuf[MAX_CMD_PART_LEN + 1];

  if(!pRangeMask || !pDest || ! pSrcStr || (insertPos < 0)
  || (pRangeMask->RangeFrom > MAXNAMELEN) || (pRangeMask->RangeTo > MAXNAMELEN) 
  || (pRangeMask->RangeFrom > pRangeMask->RangeTo))
  {
    return -1;
  }

  if((strlen(pSrcStr) + MAX_CMD_PART_LEN) > MAXNAMELEN) // TODO: Check if this is bs
  {
    return -1;
  }

  // Start with a clean target buffer
  strcpy(pDest, "");

  // Apply the beginning until the insert position
  strncat(pDest, pSrcStr, insertPos);
  pDest[insertPos] = '\0';

  // Fill the command buf
  createCommandMask(pRangeMask, commandPartBuf);

  // Apply the command buf
  strcat(pDest, commandPartBuf);
  
  // Apply the end, after the insert position
  strcat(pDest, pSrcStr + insertPos);

  return (int)(insertPos + strlen(commandPartBuf));
}

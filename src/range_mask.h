#ifndef RANGE_MASK_H
#define RANGE_MASK_H

#include <exec/types.h>

#define MAX_CMD_PART_LEN 9 // To allow [N101-106]

typedef enum
{
    RRT_NAME = 1
  , RRT_EXTENSION
} RequestedRangeType;


typedef struct RangeMask
{
  RequestedRangeType RequestedRangeType;
  ULONG RangeFrom;
  ULONG RangeTo;
} RangeMask;

/**
 * Create a command mask like [N12-16] depending on the current
 * pRangeMask values. The craeted mask is written into
 * `pDestinationCmdBuf` which must havbe at least a size of
 * MAX_CMD_PART_LEN + 1 bytes.
 */
void createCommandMask(RangeMask* pRangeMask, char* pDestinationCmdBuf);

/**
 * Fill given `pDest` by inserting a command like [N12-16] at
 * `insertPos` into given `pSrcStr`. `pSrcStr` is not changed, only part
 * wise copied into `pDest`.
 *
 * The inserted command is created depending on the `RequestedRangeType`
 * field of given `RangeMask`
 * 
 * \returns new buffer pos after the text is inserted or -1 on error.
 */
int insertRangedPart(RangeMask* pRangeMask,
                     STRPTR pDest,
                     STRPTR pSrcStr,
                     UBYTE insertPos);

#endif

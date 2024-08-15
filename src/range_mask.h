#ifndef RANGE_MASK_H
#define RANGE_MASK_H

#include <exec/types.h>

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
 * Fill given `pDest` by inserting a command like [N12-16] at
 * `insertPos` into given `pSrcStr`. `pSrcStr` is not changed, only part
 * wise copied into `pDest`.
 *
 * The inserted command is created depending on the `RequestedRangeType`
 * field of given `RangeMask`
 */
int insertRangedPart(RangeMask* pRangeMask,
                     STRPTR pDest,
                     STRPTR pSrcStr,
                     UBYTE insertPos);

#endif

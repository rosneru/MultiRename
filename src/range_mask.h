#ifndef RANGE_MASK_H
#define RANGE_MASK_H

#include <exec/types.h>

#define MAX_RANGE_STRING_LEN 10     // To allow [N101-106]

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
 *
 * Create a range mask string like "[N12-16]" from `RangeMask`.
 * The created string is written into pDestBuf` which must have at
 * least a size of MAX_RANGE_STRING_LEN + 1 bytes.
 * 
 * \returns TRUE on success and FALSE on error.
 */
BOOL createRangeMaskString(RangeMask* pRangeMask, char* pDestBuf);

/**
 * Create a range mask string like "[N12-16]" from `RangeMask` and fill
 * `pDestBuf` with `pSrcStr` with the created mask string inserted at
 * `insertPos`.
 *
 * \returns New buffer pos in `pDestBuf` at the inserted mask string or
 * -1 on error.
 */
int insertRangeMaskString(RangeMask* pRangeMask,
                          STRPTR pDestBuf,
                          ULONG destBufSize,
                          const STRPTR pSrcStr,
                          UBYTE insertPos);

#endif

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
 * The created string is written into pResultStrBuf` which must have at
 * least a size of MAX_RANGE_STRING_LEN + 1 bytes.
 * 
 * \returns TRUE on success and FALSE on error.
 */
BOOL createRangeMaskString(RangeMask* pRangeMask, char* pResultStrBuf);

/**
 * Create a range mask string like "[N12-16]" from `RangeMask` and
 * then create /fill `pResultStrBuf` with `pSrcStr` with the created 
 * mask string inserted at `insertPos`.
 *
 * \returns new buffer pos in `pResultStrBuf` at the inserted mask
 * string or -1 on error.
 */
int insertRangeMaskString(RangeMask* pRangeMask,
                          STRPTR pResultStrBuf,
                          ULONG resultBufSize,
                          const STRPTR pSrcStr,
                          UBYTE insertPos);

#endif

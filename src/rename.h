#ifndef RENAME_H
#define RENAME_H

#include "file_list.h"


typedef struct
{
  ULONG Token;
  ULONG Count;
} TokenCount;



/**
 * Create and return an array of empty TokenCount objects of size
 * `fileCount`.
 */
TokenCount* createTokenCounts(ULONG fileCount);

/**
 * Free the given `pTokenCounts`array.
 */
void freeTokenCounts(TokenCount* pTokenCounts);

#endif

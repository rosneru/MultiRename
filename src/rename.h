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


/**
 * Fill the field `NewNameToken` of every FileNode in list `pFileNodes`
 * by parsing the `pTokenCounts` array.
 */
void fillTokenOccurrences(struct List* pFilesList,
                          TokenCount* pTokenCounts,
                          ULONG numTokenCounts);


#endif

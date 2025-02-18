#ifndef RENAME_H
#define RENAME_H

#include "file_list.h"


typedef struct
{
  ULONG Token;
  ULONG Count;
} TokenCount;


/**
 * Rename every file node in given list of `FileNode*` from 
 * `pFileNode->OriginalName` to `pFileNode->NewName`.
 * 
 * Returns TRUE on success, and FALSE on error together with a detailed
 * notification of on which files the rename process idid fail 
 */
BOOL renameFiles(struct List* pFilesList, struct List* pNotifications);


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

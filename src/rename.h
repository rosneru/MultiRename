#ifndef RENAME_H
#define RENAME_H

#include "file_nodes.h"


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
BOOL renameFiles(FileNodes* pFiles,
                 struct List* pNotifications,
                 BOOL doSkipIcons);


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
 * Fill the field `NewNameToken` of every FileNode in list `pFiles`
 * by parsing the `pTokenCounts` array.
 */
void fillTokenOccurrences(FileNodes* pFiles,
                          TokenCount* pTokenCounts,
                          ULONG numTokenCounts);


#endif

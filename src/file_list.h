#ifndef FILE_LIST_H
#define FILE_LIST_H

#include <libraries/locale.h>

#include "file_node.h"
#include "notifications.h"


struct List* createFileList(void);
void freeFileList(struct List* pFilesList);

/**
 * Iterates the pFilesList and return the FileNode* that contains
 * the longest OriginalName.
 */
FileNode* getLongestOldNameNode(struct List* pFilesList);

/**
 * Iterates the pFilesList and return the FileNode* that contains
 * the longest OriginalExtension.
 */
FileNode* getLongestOldExtNode(struct List* pFilesList);

void printFileListOriginalName(struct List* pFilesList);
void printFileListNewName(struct List* pFilesList);


/**
 * Create a file note for `pFileFullPath` and add it to `pFilesList`.
 */
BOOL appendFileNode(struct List* pFilesList,
                    STRPTR pFileFullPath,
                    struct Locale* pLocale,
                    struct List* pNotifications);

/**
 * Return the path of the first FileNode in list or NULL if there
 * is none.
 */
STRPTR getFirstFilePath(struct List* pFilesList);

#endif

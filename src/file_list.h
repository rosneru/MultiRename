#ifndef FILE_LIST_H
#define FILE_LIST_H

#include "file_node.h"
#include "notifications.h"


struct List* createFileList(void);
void freeFileList(struct List* pFilesList);

/**
 * Iterates the given pFilesList and return the FileNode* that contains
 * the longest OldName.
 */
FileNode* getLongestOldNameNode(struct List* pFilesList);

void printFileListNewName(struct List* pFilesList);

BOOL appendFileNode(struct List* pFilesList,
                    STRPTR pFileFullPath,
                    struct List* pNotifications);

STRPTR getFirstFilePath(struct List* pFilesList);

#endif

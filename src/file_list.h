#ifndef FILE_LIST_H
#define FILE_LIST_H

#include "file_node.h"
#include "notifications.h"


/**
 * Creates and returns the list of file nodes according to the given
 * file names. File names whose paths are different than that one of
 * the first created node ares skipped. The number of skipped files
 * is set into the parameter variable pNumSkipped.
 * 
 * Returns NULL on error.
 */
struct List* createFileList(void);
void freeFileList(struct List* pFilesList);

void printFileListNewName(struct List* pFilesList);

BOOL appendFileNode(struct List* pFilesList,
                    STRPTR pFileFullPath,
                    struct List* pNotificationsList);

STRPTR getFirstFilePath(struct List* pFilesList);

#endif

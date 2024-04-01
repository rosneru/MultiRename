#ifndef FILE_NODE_H
#define FILE_NODE_H

#include <exec/types.h>

#include "notifications.h"

#define MAXPATHLEN 255
#define MAXNAMELEN 107  // 107 maximum in FFS, PFS

typedef struct FileNode
{
  struct Node en_Node;
  char Path[MAXPATHLEN + 1];
  char OldName[MAXNAMELEN + 1]; // Max length + \0 termination
  char NewName[MAXNAMELEN + 1];
} FileNode;

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

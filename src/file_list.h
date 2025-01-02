#ifndef FILE_LIST_H
#define FILE_LIST_H

#include <libraries/locale.h>

#include "file_node.h"
#include "notifications.h"


/**
 * Create a list to store `FileNode` items.
 */
struct List* createFileList(void);

/**
 * Destroys the nodes and then the list itself.
 * NOTE: The nodes are freed by a call of `FreeListBrowserNode()` each.
 */
void freeFileList(struct List* pFilesList);

/**
 * Counts and returns the number of `FileNode` items in this list.
 */
ULONG countFileNodes(struct List* pFilesList);

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

/**
 * Print the original name field of each `FileNode` in list.
 */
void printFileListOriginalName(struct List* pFilesList);

/**
 * Print the new name field of each `FileNode` in list.
 */
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

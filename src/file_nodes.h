#ifndef FILE_NODESHAHAHHA_H
#define FILE_NODESHAHAHHA_H

#include <exec/types.h>
#include <libraries/locale.h>

#include "file_node.h"
#include "notifications.h"

typedef struct FileNodes
{
  struct List* pList;
  BPTR FilesDirLock;  // TODO Why does BPTR not work???
} FileNodes;

/**
 * Create a list to store `FileNode` items.
 */
FileNodes* createFileNodes(void);

/**
 * Destroys the nodes and then the list itself.
 * NOTE: The nodes are freed by a call of `FreeListBrowserNode()` each.
 */
void freeFileNodes(FileNodes* pFileNodes);

/**
 * Counts and returns the number of `FileNode` items in this list.
 */
ULONG countFileNodes(FileNodes* pFileNodes);

/**
 * Iterates the pFileNodesList and return the FileNode* that contains
 * the longest OriginalName.
 */
FileNode* getLongestOldNameNode(FileNodes* pFileNodes);

/**
 * Iterates the pFileNodesList and return the FileNode* that contains
 * the longest OriginalExtension.
 */
FileNode* getLongestOldExtNode(FileNodes* pFileNodes);

/**
 * Print the original name field of each `FileNode` in list.
 */
void printFileListOriginalName(FileNodes* pFileNodes);

/**
 * Print the new name field of each `FileNode` in list.
 */
void printFileListNewName(FileNodes* pFileNodes);


/**
 * Create a file note for `pFileFullPath` and add it to `pFileNodesList`.
 */
BOOL appendFileNode(FileNodes* pFileNodes,
                    STRPTR pFileFullPath,
                    struct Locale* pLocale,
                    struct List* pNotifications);

/**
 * Return the path of the first FileNode in list or NULL if there
 * is none.
 */
STRPTR getFirstFilePath(FileNodes* pFileNodes);

#endif

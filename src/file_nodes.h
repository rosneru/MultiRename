#ifndef FILE_NODES_H
#define FILE_NODES_H

#include <exec/types.h>
#include <libraries/locale.h>

#include "file_node.h"
#include "notifications.h"

typedef struct FileNodes
{
  struct List *pList;
  BPTR DirLock;
  char DirPath[MAX_PATH_LEN + 1];
} FileNodes;

/**
 * Frees the given FileNode
 */
void freeFileNode(struct Node *pNode);

/**
 * Create a list to store `FileNode` items.
 */
FileNodes *createFileNodes(void);

/**
 * Destroys the nodes and then the list itself.
 * NOTE: The nodes are freed by a call of `FreeListBrowserNode()` each.
 */
void freeFileNodes(FileNodes *pFiles);

/**
 * Returns the lock to the files directory or ZERO if such a directory
 * hasn't been set in current session.
 */
BPTR getFilesDirLock(FileNodes *pFiles);

/**
 * Apply the given lock for the files dir. If there is already a files
 * dir lock set it is released first.
 *
 * Returns TRUE on success and FALSE on failure. Failure can occur
 * because this also creates the dir path name an there is a (rare)
 * chance the path doesn't fit into the `FileNodes::DirPath` buffer.
 * Then `IoErr()` will return `ERROR_LINE_TOO_LONG`.
 */
BOOL setFilesDirLock(FileNodes *pFiles, BPTR pFilesDirLock);

/**
 * Returns the files directory path or an empty string "" if such a
 * directory hasn't been set in current session.
 *
 * The path name is created when the lock is applied in
 * `setFilesDirLock(..)`
 */
char *getFilesDirPath(FileNodes *pFiles);

/**
 * Counts and returns the number of `FileNode` items in this list.
 */
ULONG countFileNodes(FileNodes *pFiles);

/**
 * Iterates the pFileNodesList and return the FileNode* that contains
 * the longest OriginalName.
 */
FileNode *getLongestOldNameNode(FileNodes *pFiles);

/**
 * Iterates the pFileNodesList and return the FileNode* that contains
 * the longest OriginalExtension.
 */
FileNode *getLongestOldExtNode(FileNodes *pFiles);

/**
 * Print the original name field of each `FileNode` in list.
 */
void printFileListOriginalName(FileNodes *pFiles);

/**
 * Print the new name field of each `FileNode` in list.
 */
void printFileListNewName(FileNodes *pFiles);

/**
 * Create a file note for `pFileFullPath` and add it to `pFileNodesList`.
 */
BOOL appendFileNode(FileNodes *pFiles,
  STRPTR pFileFullPath,
  struct Locale *pLocale,
  struct List *pNotifications);

/**
 * Returns TRUE if given `pFiles` list cpntains a node whose field
 * `OriginalName` is the same name as in given node `pNodeToCheck`
 */
BOOL isOriginalNameNodeAlreadyInFileNodesList(
  FileNodes *pFiles, FileNode *pNodeToCheck);

/**
 * Return the path of the first FileNode in list or NULL if there
 * is none.
 */
STRPTR getFirstFilePath(FileNodes *pFiles);

#endif

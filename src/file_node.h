#ifndef FILE_NODE_H
#define FILE_NODE_H

#include <exec/types.h>

#define MAXPATHLEN 255
#define MAXNAMELEN 107  // 107 maximum in FFS, PFS

typedef struct FileNode
{
  struct Node en_Node;
  UBYTE OldName[MAXNAMELEN + 1]; // Max length + \0 termination
  UBYTE NewName[MAXNAMELEN + 1];
} FileNode;

struct List* createFileList(UBYTE** ppFileNames);
void freeFileList(struct List* pFilesList);

#endif

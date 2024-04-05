#ifndef FILE_NODE_H
#define FILE_NODE_H

#include <exec/nodes.h>

#define MAXPATHLEN 255
#define MAXNAMELEN 107  // 107 maximum in FFS, PFS

typedef struct FileNode
{
  struct Node en_Node;
  char Path[MAXPATHLEN + 1];
  char OldName[MAXNAMELEN + 1]; // Max length + \0 termination
  char NewName[MAXNAMELEN + 1];
} FileNode;


#endif

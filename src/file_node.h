#ifndef FILE_NODE_H
#define FILE_NODE_H

#include <exec/nodes.h>
#include "date_tools.h"

#define MAXPATHLEN 255
#define MAXNAMELEN 107  // 107 maximum in FFS, PFS

typedef struct FileNode
{
  struct Node en_Node;
  char Path[MAXPATHLEN + 1];
  char OriginalName[MAXNAMELEN + 1]; // Max length + \0 termination
  UBYTE OriginalNameLen;
  UBYTE OriginalExtLen;
  DateTimeParts OriginalDate;
  char NewName[MAXNAMELEN + 1];
  UBYTE NewNameFullLen;
  BOOL IsNewNameTruncated;
} FileNode;


#endif

#ifndef FILE_NODE_H
#define FILE_NODE_H

#include "date_tools.h"
#include <exec/nodes.h>

#define MAX_PATH_LEN 2048
#define MAX_NAME_LEN 107 // 107 maximum in FFS, PFS

typedef struct FileNode
{
  struct Node en_Node;
  char Path[MAX_PATH_LEN + 1];
  char OriginalName[MAX_NAME_LEN + 1]; // Max length + \0 termination
  ULONG OriginalNameLen;
  ULONG OriginalExtLen;
  ULONG OriginalNameToken;
  DateTimeParts OriginalDate;
  char NewName[MAX_NAME_LEN + 1];
  ULONG NewNameFullLen;
  BOOL IsNewNameTruncated;
  ULONG NewNameToken;
  USHORT TokenOccurrenceNumber;
} FileNode;

#endif

#ifndef FILE_NODE_H
#define FILE_NODE_H

#include <exec/types.h>

struct List* createDummyFileList();
void freeFileList(struct List* pFilesList);

#endif

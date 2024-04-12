#ifndef RENAME_ALGORITHM_H
#define RENAME_ALGORITHM_H

#include <exec/lists.h>
#include <exec/types.h>
#include "rename_counter.h"

BOOL createNewNames(struct List* pFileNodes,
                    STRPTR pNameMask,
                    STRPTR pExtMask,
                    LONG counterStart,
                    LONG counterInc,
                    BYTE counterWidth);

#endif

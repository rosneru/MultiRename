#ifndef RENAME_ALGORITHM_H
#define RENAME_ALGORITHM_H

#include <exec/lists.h>
#include <exec/types.h>
#include "rename_counter.h"

/**
 * Iterates the given 'list of FileNodes'. For every FileNode the field
 * NewName is newly filled according the OriginalName and given parameters
 * pNameMask, pExtMask and counter settings.
 *
 */
BOOL createNewNames(struct List* pFilesList,
                    STRPTR pNameMask,
                    STRPTR pExtMask,
                    LONG counterStart,
                    LONG counterInc,
                    BYTE counterWidth);

#endif

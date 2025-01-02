#ifndef RENAME_ALGORITHM_H
#define RENAME_ALGORITHM_H

#include <exec/lists.h>
#include <exec/types.h>
#include "rename_counter.h"

/**
 * Iterates the 'pFilesList'. For every FileNode the content of field
 * NewName is calculated and set according the OriginalName and
 * parameters `pNameMask`, `pExtMask` and counter settings.
 *
 */
BOOL createNewNames(struct List* pFilesList,
                    STRPTR pNameMask,
                    STRPTR pExtMask,
                    LONG counterStart,
                    LONG counterInc,
                    BYTE counterWidth);

#endif

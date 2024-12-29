#ifndef UI_TOOLS_H
#define UI_TOOLS_H

#include <exec/types.h>
#include <intuition/classusr.h>

/**
 * Append given `pTextToAppend` to the text that is already in given
 * `pStrGadget` (which is displayed in given `pIntuiWindow`).
 *
 * Also, a temporary buffer `pScratchBuf` and ist size must be provided
 * to perform the concatenating there before writing the result to the
 * string gadget again.
 */
void appendTextToStrGadget(struct Window* pIntuiWindow,
                           Object* pStrGadget,
                           STRPTR pTextToAppend,
                           STRPTR pScratchBuf,
                           ULONG scratchBufSize);

#endif

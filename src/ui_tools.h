#ifndef UI_TOOLS_H
#define UI_TOOLS_H

#include <exec/types.h>
#include <intuition/classusr.h>

/**
 * Append `pTextToAppend` to the existing text in `pStrGadget`.
 * The parent window of the str gadget, `pIntuiWindow` must be given 
 * too.
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

/**
 * 
 */
int insertTextToStrGadget(struct Window* pIntuiWindow,
                          Object* pStrGadget,
                          STRPTR pStrToInsert,
                          ULONG insertPos,
                          STRPTR pScratchBuf,
                          ULONG scratchBufSize);

/**
 * Get the current buffer position of given string gadget `pStrGadget`.
 * Retuns -1 if buffer pos could not be retrieved.
 */
int getStrGadgetBufferPos(Object* pStrGadget);


#endif

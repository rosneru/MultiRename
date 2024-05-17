#ifndef UI_TOOLS_H
#define UI_TOOLS_H

#include <exec/types.h>
#include <intuition/classusr.h>

void appendTextToStrGadget(struct Window* pIntuiWindow,
                           Object* pStrGadget,
                           STRPTR pTextToAppend,
                           STRPTR pScratchBuf,
                           ULONG scratchBufSize);

#endif

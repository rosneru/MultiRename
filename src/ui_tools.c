#include <intuition/gadgetclass.h>

#ifdef __clang__
  #include <clib/intuition_protos.h>
#else
  #include <proto/intuition.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui_tools.h"


void appendTextToStrGadget(struct Window* pIntuiWindow,
                           Object* pStrGadget,
                           STRPTR pTextToAppend,
                           STRPTR pScratchBuf,
                           ULONG scratchBufSize)
{
  STRPTR pCurrentText;
  long bufferPos;
  ULONG remainingBufSize;
  
  if(!GetAttr(STRINGA_TextVal, pStrGadget, (ULONG*)&pCurrentText))
  {
    printf("Got no STRINGA_TextVal\n");
    return;
  }

  bufferPos = strlen(pCurrentText);
  strncpy(pScratchBuf, pCurrentText, scratchBufSize);
  pScratchBuf[scratchBufSize-1] = '\0';
  remainingBufSize = scratchBufSize - strlen(pScratchBuf);
  strncat(pScratchBuf, pTextToAppend, remainingBufSize);
  pScratchBuf[scratchBufSize-1] = '\0';

  SetGadgetAttrs((struct Gadget *) pStrGadget,
                  pIntuiWindow,
                  NULL,
                  STRINGA_BufferPos, (ULONG) bufferPos,
                  STRINGA_TextVal, (ULONG) pScratchBuf,
                  TAG_DONE);
}

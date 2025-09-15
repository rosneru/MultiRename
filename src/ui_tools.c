#include <intuition/gadgetclass.h>

#ifdef __clang__
  #include <clib/intuition_protos.h>
#else
  #include <proto/intuition.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string_tools.h"
#include "ui_tools.h"


BOOL appendTextToStrGadget(struct Window* pIntuiWindow,
                           Object* pStrGadget,
                           STRPTR pTextToAppend,
                           STRPTR pScratchBuf,
                           ULONG scratchBufSize)
{
  STRPTR pCurrentText;
  ULONG remainingBufSize;
  
  if(!GetAttr(STRINGA_TextVal, pStrGadget, (ULONG*)&pCurrentText))
  {
    return FALSE;
  }

  strncpy(pScratchBuf, pCurrentText, scratchBufSize);
  pScratchBuf[scratchBufSize-1] = '\0';
  remainingBufSize = scratchBufSize - strlen(pScratchBuf);
  strncat(pScratchBuf, pTextToAppend, remainingBufSize);
  pScratchBuf[scratchBufSize-1] = '\0';

  SetGadgetAttrs((struct Gadget *) pStrGadget,
                 pIntuiWindow,
                 NULL,
                 STRINGA_TextVal, (ULONG) pScratchBuf,
                 TAG_DONE);

  return TRUE;
}

int insertTextToStrGadget(struct Window* pIntuiWindow,
                          Object* pStrGadget,
                          STRPTR pStrToInsert,
                          ULONG insertPos,
                          STRPTR pScratchBuf,
                          ULONG scratchBufSize)
{
  STRPTR pCurrentText;
  long bufferPos = -1;

  if(!pIntuiWindow || !pStrGadget || !pStrToInsert || !pScratchBuf)
  {
    return -1;
  }

  if(!GetAttr(STRINGA_TextVal, pStrGadget, (ULONG*)&pCurrentText))
  {
    return -1;
  }

  if(strlen(pCurrentText) + strlen(pStrToInsert) + 1 > scratchBufSize)
  {
    return -1;
  }

  if(0 > (bufferPos = insertString(pCurrentText,
                                   pStrToInsert,
                                   insertPos,
                                   pScratchBuf,
                                   scratchBufSize)))
  {
    // TODO: Notify user
    return -1;
  }

  // First, set the new text into string gadget
  SetGadgetAttrs((struct Gadget *) pStrGadget,
                 pIntuiWindow,
                 NULL,
                 STRINGA_TextVal, (ULONG) pScratchBuf,
                 TAG_DONE);

  // And then set the buffer pos to insert position
  //
  //(Because setting both in one `SetGadgetAttrs` call doesn't work, as
  // `STRINGA_TextVal` always overwrites the buffer pos to the end of
  // line.)
  SetGadgetAttrs((struct Gadget *) pStrGadget,
                 pIntuiWindow,
                 NULL,
                 STRINGA_BufferPos, (ULONG) bufferPos,
                 TAG_DONE);

  return bufferPos;
}


int getStrGadgetBufferPos(Object* pStrGadget)
{
  ULONG bufferPos;

  if(!pStrGadget)
  {
    printf("pStrGadget is NULL in `getStrGadgetBufferPos()`\n");
    return -1;
  }


  if(!GetAttr(STRINGA_BufferPos, pStrGadget, &bufferPos))
  {
    return -1;
  }

  return (int)bufferPos;
}

#include <exec/lists.h>
#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
#endif

#include <string.h>

#include "rename_parser.h"

void initRenameParser(RenameParser* pParser, STRPTR pMask)
{
  if(!pParser || !pMask)
  {
    return;
  }

  NewList(&pParser->ActionList);
  pParser->Command = AC_NONE;
  pParser->CommandFrom = 0;
  pParser->CommandTo = 0;
  pParser->NumericFrom = -1;
  pParser->NumericTo = -1;
  pParser->pMask = pMask;
  pParser->MaskLen = strlen(pMask);
  pParser->MaskIndex = 0;
}

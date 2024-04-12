#include <string.h>

#include "rename_parser.h"

void initRenameParser(RenameParser* pParser, STRPTR pMask)
{
  if(!pParser || !pMask)
  {
    return;
  }

  // TODO!
  // NEWLIST(pParser->ActionList);
  pParser->Command = AC_NONE;
  pParser->CommandFrom = 0;
  pParser->CommandTo = 0;
  pParser->NumericFrom = -1;
  pParser->NumericTo = -1;
  pParser->pMask = pMask;
  pParser->MaskLen = strlen(pMask);
  pParser->MaskIndex = 0;
}

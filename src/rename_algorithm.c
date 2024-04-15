#include <exec/types.h>

#include "rename_parser.h"
#include "rename_algorithm.h"


BOOL fillNewNames(struct List* pFileNodes,
                  STRPTR pNameMask,
                  STRPTR pExtMask,
                  LONG counterStart,
                  LONG counterInc,
                  BYTE counterWidth)
{
  Counter nameCounter, extCounter;
  RenameParser nameParser, extParser;

  initCounter(&nameCounter, counterStart, counterInc, counterWidth);
  initCounter(&extCounter, counterStart, counterInc, counterWidth);

  initRenameParser(&nameParser, pNameMask);
  initRenameParser(&extParser, pExtMask);


  return TRUE;
}

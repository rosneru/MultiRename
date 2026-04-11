// clang-format off
#ifdef __clang__
  #include <clib/exec_protos.h>
  #include <clib/locale_protos.h>
#else
  #include <proto/exec.h>
  #include <proto/locale.h>
#endif
// clang-format on

#define CATCOMP_BLOCK
#include "translation.h"


struct CatCompBlockType
{
  LONG	ccb_ID;
  UWORD	ccb_StringSize;
};


/**
 * Returns the translated-into-os-language string or the default string for
 * given `stringNum`. Needs a pointer to an initialized locale struct.
 */
STRPTR tr(struct LocaleInfo * li, LONG stringNum)
{
  const struct CatCompBlockType * ccb = (APTR)CatCompBlock;
  const struct CatCompBlockType * ccb_stop = (APTR)&((BYTE *)ccb)[sizeof(CatCompBlock)];

  STRPTR builtin = NULL;
  STRPTR result = NULL;

  while(ccb < ccb_stop && ccb->ccb_StringSize > 0)
  {
    if(ccb->ccb_ID == stringNum)
    {
      builtin = (STRPTR)&ccb[1];
      break;
    }

    ccb = (struct CatCompBlockType *)&((BYTE *)ccb)[sizeof(*ccb) + ccb->ccb_StringSize];
  }

  if(li != NULL && li->li_LocaleBase != NULL)
  {
    struct Library * LocaleBase = li->li_LocaleBase;
    result = GetCatalogStr(li->li_Catalog, stringNum, builtin);
printf("GetCatalogStr: %s\n", result);
  }
  else
  {
    result = builtin;
  }

  return(result);
}

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <exec/types.h>

struct LocaleInfo
{
  APTR li_LocaleBase;
  APTR li_Catalog;
};

STRPTR tr(struct LocaleInfo *li, LONG stringNum);

#endif

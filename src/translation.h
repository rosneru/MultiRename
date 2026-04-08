#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <exec/types.h>

#define CATCOMP_NUMBERS
#include "MultiRename_catalog.h"

STRPTR tr(struct LocaleInfo *li, LONG stringNum);

#endif

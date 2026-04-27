#ifndef PARSED_ARGS_H
#define PARSED_ARGS_H

#include <exec/lists.h>
#include <libraries/locale.h>

#include "notifications.h"

#define ARG_TEMPLATE "FILES/M,PUBSCREEN/K,LONGNAMES/S,SKIPICONS/S"
enum
{
  ARG_FILES,
  ARG_PUBSCREEN,
  ARG_LONGNAMES,
  ARG_SKIPICONS,
  ARG_ARRAY_SIZE
};

#define TEMP_PATH_BUF_SIZE (MAX_PATH_LEN + 1)
typedef struct ParsedArgs
{
  char *pPubScreenName;
  BOOL AreLongNamesAllowed;
  BOOL AreIconsSkipped;
  struct DiskObject *pDiskObject;
  char pTempPathBuf[TEMP_PATH_BUF_SIZE];
} ParsedArgs;

ParsedArgs *createParsedArgs(int argc,
  char **argv,
  FileNodes *pFiles,
  struct Locale *pLocale,
  STRPTR pDirEntryMarkerName,
  struct List *pNotifications);

void freeParsedArgs(ParsedArgs *pParsedArgs);

#endif

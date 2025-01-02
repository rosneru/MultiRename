#ifndef PARSED_ARGS_H
#define PARSED_ARGS_H


#include <exec/lists.h>
#include <libraries/locale.h>

#include "notifications.h"

#define ARG_TEMPLATE "FILES/M,PUBSCREEN/K"
enum {ARG_FILES, ARG_PUBSCREEN, ARG_ARRAY_SIZE};


typedef struct ParsedArgs
{
  char* pPubScreenName;
  struct DiskObject* pDiskObject;
  char pScratchPathBuf[MAX_PATH_LEN + 1];
} ParsedArgs;

ParsedArgs* createParsedArgs(int argc,
                             char **argv,
                             struct List* pFilesList,
                             struct Locale* pLocale,
                             struct List* pNotifications);

void freeParsedArgs(ParsedArgs* pParsedArgs);


#endif

#ifndef PARSED_ARGS_H
#define PARSED_ARGS_H


#include <exec/lists.h>


#define ARG_TEMPLATE "FILES/M,PUBSCREEN/K"
enum {ARG_FILES, ARG_PUBSCREEN, ARG_ARRAY_SIZE};


typedef struct ParsedArgs
{
  STRPTR pProcessingDir;
  char* pPubScreenName;
  struct DiskObject* pDiskObject;
} ParsedArgs;

ParsedArgs* createParsedArgs(int argc, char **argv, struct List* pFilesList);
void freeParsedArgs(ParsedArgs* pParsedArgs);


#endif

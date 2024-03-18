#ifndef PARSED_ARGS_H
#define PARSED_ARGS_H

#define ARG_TEMPLATE "FILES/M,PUBSCREEN/K"
enum {ARG_FILES, ARG_PUBSCREEN, ARG_ARRAY_SIZE};


typedef struct ParsedArgs
{
  STRPTR pProcessingDir;
  STRPTR* ppFiles;
  char* pPubScreenName;
  struct DiskObject* pDiskObject;
} ParsedArgs;

ParsedArgs* createParsedArgs(int argc, char **argv);
void freeParsedArgs(ParsedArgs* pParsedArgs);


#endif

#ifndef PARSED_ARGS_H
#define PARSED_ARGS_H


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

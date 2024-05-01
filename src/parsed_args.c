#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/icon_protos.h>
#else
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/icon.h>
#endif

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "file_list.h"
#include "notifications.h"
#include "parsed_args.h"

void readCommandLineArgs(ParsedArgs* pParsedArgs,
                         struct List* pFilesList,
                         struct List* pNotifications);

void readWorkbenchArgs(ParsedArgs* pParsedArgs,
                       char **argv,
                       struct List* pFilesList,
                       struct List* pNotifications);

static struct RDArgs* pReadArgs = NULL;


ParsedArgs* createParsedArgs(int argc,
                             char **argv,
                             struct List* pFilesList,
                             struct List* pNotifications)
{
  ParsedArgs* pParsedArgs;

  if(!(pParsedArgs = AllocVec(sizeof(ParsedArgs), MEMF_CLEAR)))
  {
    PutStr("Failed to allocate memory for parsed arguments.\n");
    return NULL;
  }

  if(argc == 0)
  {
    // Started from Workbench
    readWorkbenchArgs(pParsedArgs, argv, pFilesList, pNotifications);

  }
  else
  {
    // Started from CLI
    readCommandLineArgs(pParsedArgs, pFilesList, pNotifications);
  }

  return pParsedArgs;
}

void freeParsedArgs(ParsedArgs* pParsedArgs)
{
  if(pReadArgs)
  {
    FreeArgs(pReadArgs);
  }

  if(pParsedArgs)
  {
    FreeVec(pParsedArgs);
  }

}


void readCommandLineArgs(ParsedArgs* pParsedArgs,
                         struct List* pFilesList,
                         struct List* pNotifications)
{
  BPTR lock;
  STRPTR* ppFiles;
  STRPTR pFileName;
  LONG args[ARG_ARRAY_SIZE] = {0};


  struct RDArgs* pReadArgs = ReadArgs(ARG_TEMPLATE, args, NULL);
  if(!pReadArgs)
  {
    return;
  }

  if(args[ARG_FILES] != 0)
  {
    // When the FILES argument was parsed, the ARG_FILES array item
    // points to an array of pointers to the passed FILEs (path + name).
    ppFiles = (STRPTR*) args[ARG_FILES];

    while(*ppFiles)
    {
      if((lock = Lock(*ppFiles, SHARED_LOCK)))
      {
        if(NameFromLock(lock, pParsedArgs->pScratchPathBuf, MAXPATHLEN))
        {
          appendFileNode(pFilesList,
                         pParsedArgs->pScratchPathBuf,
                         pNotifications);
        }
        else
        {
          if(IoErr() == ERROR_LINE_TOO_LONG)
          {
            // For the error notification only the file name not the
            // relative path is needed.
            pFileName = FilePart(*ppFiles);
            addNotification(pNotifications,
                            NNT_SKIPPED_PATH_TOO_LONG,
                            pFileName);
          }
        }

        UnLock(lock);
      }

      ppFiles++;
    }
  }

  if(args[ARG_PUBSCREEN] != 0)
  {
    pParsedArgs->pPubScreenName = (char*)args[ARG_PUBSCREEN];
  }
}


char* toolTypeValue(const STRPTR* ppTooltypeArray, const char* pTooltypeName)
{
  return (char*)FindToolType((CONST_STRPTR*)ppTooltypeArray, (STRPTR)pTooltypeName);
}


void readWorkbenchArgs(ParsedArgs* pParsedArgs,
                       char **argv,
                       struct List* pFilesList,
                       struct List* pNotifications)
{
  int i;
  STRPTR* ppTooltypeArray;
  STRPTR pFileName;
  BPTR oldDir;
  char* pValue;

  struct WBStartup* pWbStartup = (struct WBStartup*) argv;
  struct WBArg* pWbArg = pWbStartup->sm_ArgList;
  for(i=0; i < pWbStartup->sm_NumArgs; i++)
  {
    if((pWbArg[i].wa_Lock != 0))  // TODO check. Was 'NULL' before.
    {
      if(i == 0)
      {
        //
        // The first pWbArg is the application icon itself. Getting
        // the PUBSCREEN tooltype from it
        //

        // Change current directory the application location
        oldDir = CurrentDir(pWbArg[i].wa_Lock);

        pParsedArgs->pDiskObject = GetDiskObjectNew((STRPTR) pWbArg[i].wa_Name);

        if(NULL == pParsedArgs->pDiskObject)
        {
          ppTooltypeArray = pParsedArgs->pDiskObject->do_ToolTypes;

          pValue = toolTypeValue(ppTooltypeArray, "PUBSCREEN");
          if(pValue != NULL)
          {
            pParsedArgs->pPubScreenName = pValue;
          }
        }
        // Change current directory back to the former one
        CurrentDir(oldDir);
      }
      else
      {
        pFileName = pWbArg[i].wa_Name;
        if(NameFromLock(pWbArg[i].wa_Lock, pParsedArgs->pScratchPathBuf, MAXPATHLEN))
        {
          AddPart(pParsedArgs->pScratchPathBuf, pFileName, MAXPATHLEN);
          appendFileNode(pFilesList,
                         pParsedArgs->pScratchPathBuf,
                         pNotifications);
        }
        else
        {
          if(IoErr() == ERROR_LINE_TOO_LONG)
          {
            // For the error notification only the file name not the
            // relative path is needed.
            addNotification(pNotifications,
                            NNT_SKIPPED_PATH_TOO_LONG,
                            pFileName);
          }
        }
      }
    }
  }
}

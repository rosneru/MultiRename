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

#include "file_node.h"
#include "notifications.h"
#include "parsed_args.h"

void readCommandLineArgs(ParsedArgs* pParsedArgs,
                         struct List* pFilesList,
                         struct List* pNotificationsList);

void readWorkbenchArgs(ParsedArgs* pParsedArgs,
                       char **argv,
                       struct List* pFilesList,
                       struct List* pNotificationsList);

static struct RDArgs* pReadArgs = NULL;


ParsedArgs* createParsedArgs(int argc,
                             char **argv,
                             struct List* pFilesList,
                             struct List* pNotificationsList)
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
    readWorkbenchArgs(pParsedArgs, argv, pFilesList, pNotificationsList);

  }
  else
  {
    // Started from CLI
    readCommandLineArgs(pParsedArgs, pFilesList, pNotificationsList);
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
                         struct List* pNotificationsList)
{
  BPTR lock;
  STRPTR* ppFiles;
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
                         pNotificationsList);
        }
        else
        {
          if(IoErr() == ERROR_LINE_TOO_LONG)
          {
            addNotification(pNotificationsList,
                            NNT_SKIPPED_PATH_TOO_LONG,
                            *ppFiles);
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
                       struct List* pNotificationsList)
{
  // int i;
  // int bufLen = 2048;  // TODO How to get rid of this fixed maximum?
  // STRPTR* ppTooltypeArray;

  // STRPTR pBuf = (STRPTR) AllocVec(bufLen, MEMF_ANY);
  // if(pBuf == NULL)
  // {
  //   return;
  // }

  // struct WBStartup* pWbStartup = (struct WBStartup*) argv;
  // struct WBArg* pWbArg = pWbStartup->sm_ArgList;
  // for(i=0; i < pWbStartup->sm_NumArgs; i++)
  // {
  //   if((pWbArg[i].wa_Lock != 0))  // TODO check. Was 'NULL' before.
  //   {
  //     if(i == 0)
  //     {
  //       //
  //       // The first pWbArg is the application icon itself. Getting
  //       // the PUBSCREEN tooltype from it
  //       //

  //       // Change current directory the application location
  //       BPTR oldDir = CurrentDir(pWbArg[i].wa_Lock);

  //       pParsedArgs->pDiskObject = GetDiskObjectNew((STRPTR) pWbArg[i].wa_Name);

  //       if(NULL == pParsedArgs->pDiskObject)
  //       {
  //         ppTooltypeArray = pParsedArgs->pDiskObject->do_ToolTypes;

  //         char* pValue = toolTypeValue(ppTooltypeArray, "PUBSCREEN");
  //         if(pValue != NULL)
  //         {
  //           pParsedArgs->pPubScreenName = pValue;
  //         }
  //       }
  //       // Change current directory back to the former one
  //       CurrentDir(oldDir);
  //     }
  //     else if(i < 3)
  //     {
  //       if(NameFromLock(pWbArg[i].wa_Lock, pBuf, bufLen) != 0)
  //       {
  //         printf("%s\n", pBuf);
  //         if(AddPart(pBuf,(STRPTR) pWbArg[i].wa_Name, bufLen))
  //         {
  //           printf("    %s\n", pBuf);
  //           // if(i == 1)
  //           // {
  //           //   m_LeftFilePath = pBuf;
  //           // }
  //           // else
  //           // {
  //           //   m_RightFilePath = pBuf;
  //           // }
  //         }
  //       }
  //     }
  //     else
  //     {
  //       // We only need the filenames of the first 2 selected icons
  //       break;
  //     }
  //   }
  // }

  // FreeVec(pBuf);
}

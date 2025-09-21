// clang-format off
#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/icon_protos.h>
#else
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/icon.h>
#endif
// clang-format on

#include <dos/dos.h>
#include <exec/libraries.h>
#include <exec/types.h>
#include <libraries/locale.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "file_nodes.h"
#include "file_tools.h"
#include "notifications.h"
#include "parsed_args.h"

/// Forwards / private function declarations

void readCommandLineArgs(ParsedArgs *pParsedArgs,
  FileNodes *pFiles,
  struct Locale *pLocale,
  struct List *pNotifications);

void readWorkbenchArgs(ParsedArgs *pParsedArgs,
  char **argv,
  FileNodes *pFiles,
  struct Locale *pLocale,
  struct List *pNotifications);

static struct RDArgs *pReadArgs = NULL;

///
/// Public function implementations

ParsedArgs *createParsedArgs(int argc,
  char **argv,
  FileNodes *pFiles,
  struct Locale *pLocale,
  struct List *pNotifications)
{
  ParsedArgs *pParsedArgs;

  if (!(pParsedArgs = AllocVec(sizeof(ParsedArgs), MEMF_CLEAR)))
  {
    PutStr("Failed to allocate memory for parsed arguments.\n");
    return NULL;
  }

  if (argc == 0)
  {
    // Started from Workbench
    readWorkbenchArgs(pParsedArgs, argv, pFiles, pLocale, pNotifications);
  }
  else
  {
    // Started from CLI
    readCommandLineArgs(pParsedArgs, pFiles, pLocale, pNotifications);
  }

  return pParsedArgs;
}

void freeParsedArgs(ParsedArgs *pParsedArgs)
{
  if (pReadArgs)
  {
    FreeArgs(pReadArgs);
  }

  if (pParsedArgs)
  {
    FreeVec(pParsedArgs);
  }
}

///
/// Private function implementations

void readCommandLineArgs(ParsedArgs *pParsedArgs,
  FileNodes *pFiles,
  struct Locale *pLocale,
  struct List *pNotifications)
{
  BPTR lock;
  STRPTR *ppFiles;
  STRPTR pFileName;
  LONG args[ARG_ARRAY_SIZE] = { 0 };

  struct RDArgs *pReadArgs = ReadArgs(ARG_TEMPLATE, args, NULL);
  if (!pReadArgs)
  {
    return;
  }

  if (args[ARG_FILES] != 0)
  {
    // When the FILES argument was parsed, the ARG_FILES array item
    // points to an array of pointers to the passed FILEs (path + name).
    ppFiles = (STRPTR *)args[ARG_FILES];

    while (*ppFiles)
    {
      if ((lock = lockFromLongName(*ppFiles)))
      {
        if (NameFromLock(lock, pParsedArgs->pTempPathBuf, MAX_PATH_LEN))
        {
          if (!getFilesDirLock(pFiles))
          {
            setFilesDirLock(pFiles, ParentDir(lock));
          }

          appendFileNode(
            pFiles, pParsedArgs->pTempPathBuf, pLocale, pNotifications);
        }
        else
        {
          if (IoErr() == ERROR_LINE_TOO_LONG)
          {
            // For the error notification only the file name not the
            // path is needed.
            pFileName = FilePart(*ppFiles);
            addNotification(
              pNotifications, NNT_SKIPPED_PATH_TOO_LONG, pFileName);
          }
        }

        UnLock(lock);
      }
      else
      {
        addNotification(pNotifications, NNT_SKIPPED_FAILED_LOCK, *ppFiles);
      }

      ppFiles++;
    }
  }

  if (args[ARG_PUBSCREEN] != 0)
  {
    pParsedArgs->pPubScreenName = (char *)args[ARG_PUBSCREEN];
  }

  pParsedArgs->AreLongNamesAllowed = args[ARG_LONGNAMES] != 0;
  pParsedArgs->AreIconsSkipped = args[ARG_SKIPICONS] != 0;
}

char *toolTypeValue(const STRPTR *ppTooltypeArray, const char *pTooltypeName)
{
  return (char *)FindToolType(
    (CONST_STRPTR *)ppTooltypeArray, (STRPTR)pTooltypeName);
}

void readWorkbenchArgs(ParsedArgs *pParsedArgs,
  char **argv,
  FileNodes *pFiles,
  struct Locale *pLocale,
  struct List *pNotifications)
{
  int i;
  STRPTR *ppTooltypeArray;
  STRPTR pFileName;
  BPTR oldDir;
  char *pValue;

  struct WBStartup *pWbStartup = (struct WBStartup *)argv;
  struct WBArg *pWbArg = pWbStartup->sm_ArgList;

  for (i = 0; i < pWbStartup->sm_NumArgs; i++)
  {
    if ((pWbArg[i].wa_Lock != 0)) // TODO check. Was 'NULL' before.
    {
      if (i == 0)
      {
        //
        // The first pWbArg is the application icon itself. Getting
        // the tooltypes from it
        //

        // Change current directory the application location
        oldDir = CurrentDir(pWbArg[i].wa_Lock);

        if ((pParsedArgs->pDiskObject =
                GetDiskObjectNew((STRPTR)pWbArg[i].wa_Name)))
        {
          ppTooltypeArray = pParsedArgs->pDiskObject->do_ToolTypes;

          pValue = toolTypeValue(ppTooltypeArray, "PUBSCREEN");
          if (pValue != NULL)
          {
            pParsedArgs->pPubScreenName = pValue;
          }

          pParsedArgs->AreLongNamesAllowed =
            toolTypeValue(ppTooltypeArray, "LONGNAMES") != NULL;
          pParsedArgs->AreIconsSkipped =
            toolTypeValue(ppTooltypeArray, "SKIPICONS") != NULL;
        }
        // Change current directory back to the former one
        CurrentDir(oldDir);
      }
      else
      {
        pFileName = pWbArg[i].wa_Name;
        if (NameFromLock(
              pWbArg[i].wa_Lock, pParsedArgs->pTempPathBuf, MAX_PATH_LEN))
        {
          if (!getFilesDirLock(pFiles))
          {
            setFilesDirLock(pFiles, DupLock(pWbArg[i].wa_Lock));
          }

          AddPart(pParsedArgs->pTempPathBuf, pFileName, MAX_PATH_LEN);
          appendFileNode(
            pFiles, pParsedArgs->pTempPathBuf, pLocale, pNotifications);
        }
        else
        {
          if (IoErr() == ERROR_LINE_TOO_LONG)
          {
            // For the error notification only the file name not the
            // relative path is needed.
            addNotification(
              pNotifications, NNT_SKIPPED_PATH_TOO_LONG, pFileName);
          }
        }
      }
    }
  }
}

///

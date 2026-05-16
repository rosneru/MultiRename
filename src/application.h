#ifndef APPLICATION_H
#define APPLICATION_H

#include <exec/lists.h>
#include <exec/ports.h>
#include <exec/types.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <libraries/locale.h>

#include "file_nodes.h"
#include "notifications.h"
#include "parsed_args.h"
#include "range_mask.h"
#include "range_select_window.h"

#include "localization.h"

#define TEMP_BUF_SIZE 1024

typedef struct Application
{
  struct Locale *pLocale;
  struct MsgPort *pAppWindowPort;
  struct List *pNotifications;
  ParsedArgs *pParsedArgs;
  FileNodes *pFiles;
  ULONG FilesCount;
  Object *pWinObject;
  RangeMask RangeMask;
  RangeSelectWindow *pRangeSelectWindow;
  struct Window *pIntuiWindow;
  struct Screen *pPubScreen;
  STRPTR pAboutMessage;
  ULONG SigMask;
  BOOL IsExitRequested;
  BOOL IsResetNeeded;
  BOOL IsStartAllowed;
  char WindowTitle[MAX_PATH_LEN + 32];
  char TempBuf[TEMP_BUF_SIZE + 1];
  long NameGadgetBufferPos;
  long ExtGadgetBufferPos;
  struct LocaleInfo *pLocaleInfo;
} Application;

Application *createApplication(
  int argc, char **argv, struct LocaleInfo* pLocaleInfo);
void disposeApplication(Application *pApp);

BOOL runApplication(Application *pApp);

#endif

#ifndef APPLICATION_H
#define APPLICATION_H

#include <exec/lists.h>
#include <exec/ports.h>
#include <exec/types.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <libraries/locale.h>

#include "file_list.h"
#include "notifications.h"
#include "parsed_args.h"
#include "range_mask.h"
#include "range_select_window.h"

#define SCRATCH_BUF_SIZE 1024

typedef struct Application
{
  struct Locale* pLocale;
  struct MsgPort* pAppWindowPort;
  struct List* pNotifications;
  ParsedArgs* pParsedArgs;
  struct List* pFiles;
  Object* pWinObject;
  RangeMask RangeMask;
  RangeSelectWindow* pRangeSelectWindow;
  struct Window* pIntuiWindow;
  ULONG SigMask;
  char FilesPath[MAXPATHLEN + 1];
  char WindowTitle[MAXPATHLEN + 32];
  char ScratchBuf[SCRATCH_BUF_SIZE];
} Application;

Application* createApplication(int argc, char **argv);
void disposeApplication(Application* pApp);

BOOL runApplication(Application* pApp);

#endif

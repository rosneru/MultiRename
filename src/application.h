#ifndef APPLICATION_H
#define APPLICATION_H

#include <exec/lists.h>
#include <exec/types.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>

#include "file_node.h"
#include "notifications.h"
#include "parsed_args.h"

#define SCRATCH_BUF_SIZE 1024

typedef struct Application
{
  struct List* pNotificationsList;
  ParsedArgs* pParsedArgs;
  struct List* pFileList;
  Object* pWinObject;
  struct Window* pIntuiWindow;
  char FilesPath[MAXPATHLEN + 1];
  char WindowTitle[MAXPATHLEN + 32];
  char ScratchBuf[SCRATCH_BUF_SIZE];
} Application;

Application* createApplication(int argc, char **argv);
void disposeApplication(Application* pApp);

BOOL runApplication(Application* pApp);


/**
 * Set current files path as window title
 */
void updateApplicationWindowTitle(Application* pApp);

#endif

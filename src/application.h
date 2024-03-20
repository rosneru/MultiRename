#ifndef APPLICATION_H
#define APPLICATION_H

#include <intuition/classusr.h>
#include <intuition/intuition.h>

#include "file_node.h"
#include "parsed_args.h"

typedef struct Application
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  struct List* pFileList;
  char FilesPath[MAXPATHLEN + 1];
  char WindowTitle[MAXPATHLEN + 32];
  char ScratchBuf[1024];
  ParsedArgs* pParsedArgs;
} Application;

Application* createApplication(int argc, char **argv);
void disposeApplication(Application* pApp);

BOOL runApplication(Application* pApp);


/**
 * Set current files path as window title
 */
void updateApplicationWindowTitle(Application* pApp);

#endif

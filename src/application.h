#ifndef APPLICATION_H
#define APPLICATION_H

#include <intuition/classusr.h>
#include <intuition/intuition.h>

#include "parsed_args.h"

typedef struct Application
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  struct List* pFileList;
  ParsedArgs* pParsedArgs;
} Application;


Application* createApplication(int argc, char **argv);
void disposeApplication(Application* pApp);

BOOL runApplication(Application* pApp);


#endif

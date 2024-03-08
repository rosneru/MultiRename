#ifndef APPLICATION_H
#define APPLICATION_H

#include <intuition/classusr.h>
#include <intuition/intuition.h>

typedef struct Application
{
  Object* pWinObject;
  struct Window* pIntuiWindow;
  struct List* pFileList;
} Application;


Application* createApplication(void);
void disposeApplication(Application* pApp);

BOOL runApplication(Application* pApp);


#endif

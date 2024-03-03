#include <stdlib.h>

#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/types.h>

#include <workbench/startup.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/dos_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
#endif

#include "application.h"

/**
 * The auto-open code of gcc uses the wrong names for some library bases
 * and then fails to open them:
 *   "window.library" (wrong) instead of "window.class" (right)
 *   "label.gadget" (wrong) instead  of "images/label.image" (right)
 *
 * To fix this, (only) these libraries are manually opened and closed.
 * But first their library base variables must be initialized here.
 * That means NULL must be assigned to the variable names.
 *
 * See: https://eab.abime.net/showpost.php?p=1490638&postcount=1285
 */
// struct IntuitionBase* IntuitionBase = NULL;
struct Library* WindowBase = NULL;
struct Library* LabelBase = NULL;


void intuiEventLoop(Object* pWindowObject);




BOOL ensureOpenLibs(void)
{
  if(WindowBase = OpenLibrary("window.class", 47L))
  {
    if(LabelBase = OpenLibrary("images/label.image", 47L))
    {
      return TRUE;
    }
    else
    {
      PutStr("Failed to load v47 label.image.\n");
    }
  }
  else
  {
    PutStr("Failed to load v47 window.class.\n");
  }

  return FALSE;
}

void ensureCloseLibs(void)
{
  if(LabelBase != NULL)
  {
    CloseLibrary(LabelBase);
  }
  
  if(WindowBase != NULL)
  {
    CloseLibrary(WindowBase);
  }
}


/**
 * CLI entry point
 */
int main(int argc, char **argv)
{
  Application* pApp;
  ULONG result = RETURN_FAIL;
  if(TRUE == ensureOpenLibs())
  {
    if((pApp = createApplication()))
    {
      if(TRUE == runApplication(pApp))
      {
        result = RETURN_OK;
      }
    }
  }

  ensureCloseLibs();
  exit(result);
}


/**
 * Workbench entry point.
 */
void wbmain(struct WBStartup* wb)
{
  // Call the CLI entry point with argc=0
  main(0, (char **) wb);
}

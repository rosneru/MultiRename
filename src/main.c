#include <stdlib.h>

#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/types.h>

#include <workbench/startup.h>

#ifdef __clang__
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#else
#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#endif

#include "application.h"

/// Forwards / private function declarations

BOOL openLibs(void);
void closeLibs(void);

///
/// Private variables

/**
 * There are 2 problems regarding the use of Reaction BOOPSIES:
 *
 * 1) The auto-open code of gcc uses the wrong names for some library
 * bases and then fails to open them:
 *   "window.library" (wrong) instead of "window.class" (right)
 *   "label.gadget" (wrong) instead  of "images/label.image" (right)
 *
 * 2) SAS-C is only able to auto-open the reaction BOOPSIE library bases
 * when linking against the recation.lib, what according the OS
 * developers currently (2024) shouldn't be done fpr new projects.
 *
 * To fix both problems, the libraries are manually opened and closed.
 * But first their library base variables must be initialized here.
 * And initialization means NULL must be assigned to the variable names,
 * See: https://eab.abime.net/showpost.php?p=1490638&postcount=1285
 */
struct IntuitionBase *IntuitionBase = NULL;
struct Library *WindowBase = NULL;
struct Library *LayoutBase = NULL;
struct Library *BevelBase = NULL;
struct Library *ButtonBase = NULL;
struct Library *ChooserBase = NULL;
struct Library *IntegerBase = NULL;
struct Library *ListBrowserBase = NULL;
struct Library *LabelBase = NULL;
struct Library *SliderBase = NULL;
struct Library *StringBase = NULL;

///
/// Public function implementations

/**
 * CLI entry point
 */
int main(int argc, char **argv)
{
  Application *pApp;
  ULONG result = RETURN_FAIL;
  if (TRUE == openLibs())
  {
    if ((pApp = createApplication(argc, argv)))
    {
      if (TRUE == runApplication(pApp))
      {
        result = RETURN_OK;
      }

      disposeApplication(pApp);
    }
  }

  closeLibs();
  exit(result);
}

/**
 * Workbench entry point.
 */
void wbmain(struct WBStartup *wb)
{
  // Call the CLI entry point with argc=0
  main(0, (char **)wb);
}

///
/// Private function implementations

BOOL openLibs(void)
{
  if (NULL
    == (IntuitionBase =
          (struct IntuitionBase *)OpenLibrary("intuition.library", 47)))
  {
    PutStr("Failed to open intuition.library v47.\n");
    return FALSE;
  }

  if (NULL == (WindowBase = OpenLibrary("window.class", 47)))
  {
    PutStr("Failed to open window.class v47.\n");
    return FALSE;
  }

  if (NULL == (LayoutBase = OpenLibrary("gadgets/layout.gadget", 47)))
  {
    PutStr("Failed to open layout.gadget v47.\n");
    return FALSE;
  }

  if (NULL == (BevelBase = OpenLibrary("images/bevel.image", 47)))
  {
    PutStr("Failed to open bevel.image v47.\n");
    return FALSE;
  }

  if (NULL == (ButtonBase = OpenLibrary("gadgets/button.gadget", 47)))
  {
    PutStr("Failed to open button.gadget v47.\n");
    return FALSE;
  }

  if (NULL == (ChooserBase = OpenLibrary("gadgets/chooser.gadget", 47)))
  {
    PutStr("Failed to open chooser.gadget v47.\n");
    return FALSE;
  }

  if (NULL == (IntegerBase = OpenLibrary("gadgets/integer.gadget", 47)))
  {
    PutStr("Failed to open integer.gadget v47.\n");
    return FALSE;
  }

  if (NULL == (ListBrowserBase = OpenLibrary("gadgets/listbrowser.gadget", 47)))
  {
    PutStr("Failed to open listbrowser.gadget v47.\n");
    return FALSE;
  }

  if (NULL == (LabelBase = OpenLibrary("images/label.image", 47)))
  {
    PutStr("Failed to open label.image v47.\n");
    return FALSE;
  }

  if (NULL == (SliderBase = OpenLibrary("gadgets/slider.gadget", 47)))
  {
    PutStr("Failed to open slider.gadget v47.\n");
    return FALSE;
  }

  if (NULL == (StringBase = OpenLibrary("gadgets/string.gadget", 47)))
  {
    PutStr("Failed to open string.gadget v47.\n");
    return FALSE;
  }

  return TRUE;
}

void closeLibs(void)
{
  if (NULL != StringBase)
  {
    CloseLibrary(StringBase);
  }

  if (NULL != SliderBase)
  {
    CloseLibrary(SliderBase);
  }

  if (NULL != LabelBase)
  {
    CloseLibrary(LabelBase);
  }

  if (NULL != ListBrowserBase)
  {
    CloseLibrary(ListBrowserBase);
  }

  if (NULL != IntegerBase)
  {
    CloseLibrary(IntegerBase);
  }

  if (NULL != ChooserBase)
  {
    CloseLibrary(ChooserBase);
  }

  if (NULL != ButtonBase)
  {
    CloseLibrary(ButtonBase);
  }

  if (NULL != BevelBase)
  {
    CloseLibrary(BevelBase);
  }

  if (NULL != LayoutBase)
  {
    CloseLibrary(LayoutBase);
  }

  if (NULL != WindowBase)
  {
    CloseLibrary(WindowBase);
  }

  if (NULL != IntuitionBase)
  {
    CloseLibrary((struct Library *)IntuitionBase);
  }
}

///

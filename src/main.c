#include <stdlib.h>

#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/types.h>

#include <workbench/startup.h>

// clang-format off
#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/locale_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/locale.h>
#endif
// clang-format on

#include "application.h"
#include "translation.h"

/// Forwards / private function declarations

BOOL openLibs(struct LocaleInfo* pLocaleInfo);
void closeLibs(struct LocaleInfo* pLocaleInfo);

///
/// Private variables

/**
 * There are 2 problems regarding the use of Reaction BOOPSIES:
 *
 * 1) The auto-open code of gcc uses the wrong names for some library bases and
 *    then fails to open them:
 *   "window.library" (wrong) instead of "window.class" (right)
 *   "label.gadget" (wrong) instead  of "images/label.image" (right)
 *
 * 2) SAS-C is only able to auto-open the reaction BOOPSIE library bases when
 *    linking against the reaction.lib, what according the OS developers
 *    currently (2024) shouldn't be done fpr new projects.
 *
 * To fix both problems, the libraries are manually opened and closed. But first
 * their library base variables must be initialized here. And initialization
 * means NULL must be assigned to the variable names, See:
 * https://eab.abime.net/showpost.php?p=1490638&postcount=1285
 */
struct IntuitionBase *IntuitionBase = NULL;
struct LocaleBase  *LocaleBase;
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
  struct LocaleInfo localeInfo;
  if (openLibs(&localeInfo))
  {
    if ((pApp = createApplication(argc, argv, &localeInfo)))
    {
      if (runApplication(pApp))
      {
        result = RETURN_OK;
      }

      disposeApplication(pApp);
    }
  }

  closeLibs(&localeInfo);
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

BOOL openLibs(struct LocaleInfo* pLocaleInfo)
{
  if(!pLocaleInfo)
  {
    PutStr("Internal error: got no LocaleInfo.\n");
    return FALSE;
  }

  if (LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library", 39))
  {
    pLocaleInfo->li_LocaleBase = LocaleBase;
    pLocaleInfo->li_Catalog = OpenCatalog(NULL, "MultiRename.catalog", TAG_DONE);
  }

  if (!(IntuitionBase =
          (struct IntuitionBase *)OpenLibrary("intuition.library", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_INTUI));
    return FALSE;
  }

  if (!(WindowBase = OpenLibrary("window.class", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_WINCLASS));
    return FALSE;
  }

  if (!(LayoutBase = OpenLibrary("gadgets/layout.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_LAYOUTGAD));
    return FALSE;
  }

  if (!(BevelBase = OpenLibrary("images/bevel.image", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_BEVELIMG));
    return FALSE;
  }

  if (!(ButtonBase = OpenLibrary("gadgets/button.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_BUTTONGAD));
    return FALSE;
  }

  if (!(ChooserBase = OpenLibrary("gadgets/chooser.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_CHOOSERGAD));
    return FALSE;
  }

  if (!(IntegerBase = OpenLibrary("gadgets/integer.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_INTEGERGAD));
    return FALSE;
  }

  if (!(ListBrowserBase = OpenLibrary("gadgets/listbrowser.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_LSTBRWSGAD));
    return FALSE;
  }

  if (!(LabelBase = OpenLibrary("images/label.image", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_LABELIMG));
    return FALSE;
  }

  if (!(SliderBase = OpenLibrary("gadgets/slider.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_SLIDERGAD));
    return FALSE;
  }

  if (!(StringBase = OpenLibrary("gadgets/string.gadget", 47)))
  {
    PutStr(tr(pLocaleInfo, MSG_FAILED_OPEN_STRINGGAD));
    return FALSE;
  }

  return TRUE;
}

void closeLibs(struct LocaleInfo* pLocaleInfo)
{
  if (StringBase)
  {
    CloseLibrary(StringBase);
  }

  if (SliderBase)
  {
    CloseLibrary(SliderBase);
  }

  if (LabelBase)
  {
    CloseLibrary(LabelBase);
  }

  if (ListBrowserBase)
  {
    CloseLibrary(ListBrowserBase);
  }

  if (IntegerBase)
  {
    CloseLibrary(IntegerBase);
  }

  if (ChooserBase)
  {
    CloseLibrary(ChooserBase);
  }

  if (ButtonBase)
  {
    CloseLibrary(ButtonBase);
  }

  if (BevelBase)
  {
    CloseLibrary(BevelBase);
  }

  if (LayoutBase)
  {
    CloseLibrary(LayoutBase);
  }

  if (WindowBase)
  {
    CloseLibrary(WindowBase);
  }

  if (IntuitionBase)
  {
    CloseLibrary((struct Library *)IntuitionBase);
  }

  if(!pLocaleInfo)
  {
    PutStr("Internal error: got no LocaleInfo.\n");
    return;
  }

  if(pLocaleInfo->li_Catalog)
  {
    CloseCatalog(pLocaleInfo->li_Catalog);
  }

  if(pLocaleInfo->li_LocaleBase)
  {
    CloseLibrary((struct Library*)LocaleBase);
  }
}

///

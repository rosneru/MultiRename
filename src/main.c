/**
 * Compile: sc main.c LINK NOSTACKCHECK 
 */

#include <classes/window.h>
#include <gadgets/layout.h>
#include <intuition/classusr.h>
#include <stdlib.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/intuition_protos.h>
  #include <clib/layout_protos.h>
  #include <clib/window_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
  #include <proto/intuition.h>
  #include <proto/layout.h>
  #include <proto/window.h>
#endif


/**
 * Initialize Library bases with NULL to avoid auto opening attempt by 
 * gcc using the wrong library names (wrong: "window.library" instead 
 * of right: "window.class" etc.)
 * 
 * See: https://eab.abime.net/showpost.php?p=1490638&postcount=1285
 */
struct IntuitionBase* IntuitionBase = NULL;
struct Library* WindowBase = NULL;
struct Library* LayoutBase = NULL;

void cleanExit(Object* pWindowObject);
void processEvents(Object* pWindowObject);


int main(void)
{
  struct Window* pIntuiWin = NULL;
  Object* pWindowObject = NULL;
  Object* pMainLayout = NULL;
  
  if (NULL == (IntuitionBase = (struct IntuitionBase*)
                                OpenLibrary("intuition.library", 47)))
  {
    cleanExit(NULL);
  }

  if (NULL == (WindowBase = OpenLibrary("window.class", 0L)))
  {
    cleanExit(NULL);
  }

  if (NULL == (LayoutBase = OpenLibrary("gadgets/layout.gadget", 0L)))
  {
    cleanExit(NULL);
  }

  if (NULL == (pMainLayout = NewObject(LAYOUT_GetClass(), NULL,
                                       LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
                                       LAYOUT_DeferLayout, TRUE,
                                       LAYOUT_SpaceInner, TRUE,
                                       LAYOUT_SpaceOuter, TRUE,
                                       TAG_DONE)))
  {
    cleanExit(NULL);
  }

  if (NULL == (pWindowObject = NewObject(WINDOW_GetClass(), NULL, 
                                         WINDOW_Position, WPOS_CENTERSCREEN,
                                         WA_Activate, TRUE,
                                         WA_Title, "BOOPSI window demo",
                                         WA_DragBar, TRUE,
                                         WA_CloseGadget, TRUE,
                                         WA_DepthGadget, TRUE,
                                         WA_SizeGadget, TRUE,
                                         WA_InnerWidth, 300,
                                         WA_InnerHeight, 150,
                                         WA_IDCMP, IDCMP_CLOSEWINDOW,
                                         WINDOW_Layout, pMainLayout,
                                         TAG_DONE)))
  {
    cleanExit(NULL);
  }

  if (NULL == (pIntuiWin = (struct Window*)DoMethod(pWindowObject, WM_OPEN, NULL)))
  {
    cleanExit(pWindowObject);
  }

  processEvents(pWindowObject);
  DoMethod(pWindowObject, WM_CLOSE);
  cleanExit(pWindowObject);
}


void processEvents(Object* pWindowObject)
{
  ULONG winSig;
  ULONG receivedSig;
  ULONG result;
  ULONG code;
  BOOL end = FALSE;

  GetAttr(WINDOW_SigMask, pWindowObject, &winSig);

  while (FALSE == end)
  {
    receivedSig = Wait(winSig);
    while ((result = DoMethod(pWindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
    {
      switch (result & WMHI_CLASSMASK)
      {
      case WMHI_CLOSEWINDOW:
        end = TRUE;
        break;
      }
    }
  }
}

void cleanExit(Object* pWindowObject)
{
  if (pWindowObject)
  {
    DisposeObject(pWindowObject);
  }

  CloseLibrary((struct Library*)IntuitionBase);
  CloseLibrary(WindowBase);
  CloseLibrary(LayoutBase);
  exit(0);
}

/**
 * Compile: sc main.c LINK NOSTACKCHECK 
 */
#include <stdlib.h>

#include <classes/window.h>
#include <gadgets/layout.h>
#include <intuition/classusr.h>

#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>


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

	pMainLayout = VGroupObject,
		ICA_TARGET, ICTARGET_IDCMP,

		LAYOUT_SpaceOuter, TRUE,
		LAYOUT_BevelStyle, BVS_GROUP,
		LAYOUT_DeferLayout, TRUE,	/* this tag instructs layout.gadget to
									 * defer GM_LAYOUT and GM_RENDER and ask
									 * the application to do them. This
									 * lessens the load on input.device
									 */
		LAYOUT_AddChild, HGroupObject,
				LAYOUT_SpaceOuter, FALSE,
				LAYOUT_AddChild, HGroupObject,
						LAYOUT_SpaceOuter, TRUE,
						/* the first group is three label-less buttons
						 * side by side
						 */
						LAYOUT_BevelStyle, BVS_GROUP,
						LAYOUT_Label, "Horizontal",
						LAYOUT_AddChild, ButtonObject,
							End,
						LAYOUT_AddChild, ButtonObject,
							End,
						LAYOUT_AddChild, ButtonObject,
							End,
					End,

				LAYOUT_AddChild, VGroupObject,
						LAYOUT_SpaceOuter, TRUE,
						/* the second group is three label-less buttons
						 * in a vertical group
						 */
						LAYOUT_BevelStyle, BVS_GROUP,
						LAYOUT_Label, "Vertical",
						LAYOUT_AddChild, ButtonObject,
							End,
						LAYOUT_AddChild, ButtonObject,
							End,
						LAYOUT_AddChild, ButtonObject,
							End,
					End,
			End,

		LAYOUT_AddChild, HGroupObject,
				/* four buttons of varying widths */
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_Label, "Free, Fixed and Weighted sizes.",
				LAYOUT_AddChild, ButtonObject,
						GA_Text, "25Kg",
					End,
					CHILD_WeightedWidth, 25,

				LAYOUT_AddChild, ButtonObject,
						GA_Text, "50Kg",
					End,
					CHILD_WeightedWidth, 50,

				LAYOUT_AddChild, ButtonObject,
						GA_Text, "75Kg",
					End,
					CHILD_WeightedWidth, 75,

				LAYOUT_AddChild, ButtonObject,
						GA_Text, "100Kg",
					End,
					CHILD_WeightedWidth, 100,
			End,
			CHILD_WeightedHeight,0,

		LAYOUT_AddChild, HGroupObject,
				/* four buttons sized in another way */
				LAYOUT_AddChild, ButtonObject,
						GA_Text, "Free",
					End,

				LAYOUT_AddChild, ButtonObject,
						GA_Text, "Fixed",
					End,
					CHILD_WeightedWidth, 0,

				LAYOUT_AddChild, ButtonObject,
						GA_Text, "Free",
					End,

				LAYOUT_AddChild, ButtonObject,
						GA_Text, "Fixed",
					End,
					CHILD_WeightedWidth, 0,
			End,
			CHILD_WeightedHeight,0,
			CHILD_MinWidth, 300,

		End;

  if (NULL == pMainLayout)
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

  if (IntuitionBase)
  {
    CloseLibrary((struct Library*)IntuitionBase);
  }

  if (WindowBase)
  {
    CloseLibrary(WindowBase);
  }

  if (LayoutBase)
  {
    CloseLibrary(LayoutBase);
  }

  exit(0);
}

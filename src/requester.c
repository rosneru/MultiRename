#include <classes/window.h>
#include <libraries/asl.h>

#ifdef __clang__
  #include <clib/asl_protos.h>
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/intuition_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/asl.h>
  #include <proto/exec.h>
  #include <proto/intuition.h>
#endif

#include "requester.h"

long showEasyRequest(Object* pWinObject,
                     struct Window* pWindow,
                     char* pTitle,
                     char* pButtonTexts,
                     char* pMessage)
{
  ULONG requestWindowFlags = 0, activeWindowFlags = 0;
  struct Window* pRequesterWindow;
  struct Requester sleepRequester;
  ULONG code, result;
  long selected = -1;

  struct EasyStruct easyStruct =
  {
    sizeof(struct EasyStruct),
    0,
    pTitle,
    pMessage,
    pButtonTexts
  };

  if(!(pRequesterWindow = BuildEasyRequestArgs(pWindow, &easyStruct, 0, NULL)))
  {
    return 0;
  }
  
  // Block the window that this requester is tied to
  InitRequester(&sleepRequester);
  Request(&sleepRequester, pWindow);
  SetWindowPointer(pWindow, WA_BusyPointer, TRUE, TAG_DONE);

  // // Show the requester ("message box")
  // requestResult = EasyRequestArgs(pWindow, &easyStruct, NULL, "");
  requestWindowFlags = 1UL << pRequesterWindow->UserPort->mp_SigBit;
  activeWindowFlags = 1UL << pWindow->UserPort->mp_SigBit;

  do
  {
    ULONG flags = Wait(requestWindowFlags | activeWindowFlags);
    if (flags & activeWindowFlags)
    {
      while ((result = DoMethod(pWinObject, WM_HANDLEINPUT, &code)))
      {
        switch (result & WMHI_CLASSMASK)
        {
          case WMHI_NEWSIZE:
          {
            DoMethod(pWinObject, WM_RETHINK);
            break;
          }
        }
      }
      // struct IntuiMessage* pMsg;
      // while ((pMsg = GT_GetIMsg(pActiveWindow->UserPort)) != NULL)
      // {
      //   switch (pMsg->Class)
      //   {
      //     // One of the windows has been resized
      //     case IDCMP_NEWSIZE:
      //       for(size_t i = 0; i < m_pAllWindowsVector->size(); i++)
      //       {
      //         if((*m_pAllWindowsVector)[i]->getIntuiWindow() == pMsg->IDCMPWindow)
      //         {
      //           // Re-paint the resized window
      //           (*m_pAllWindowsVector)[i]->performResize();
      //           break;
      //         }
      //       }
      //       break;

      //     // One of the windows must be refreshed
      //     case IDCMP_REFRESHWINDOW:
      //       GT_BeginRefresh(pMsg->IDCMPWindow);
      //       GT_EndRefresh(pMsg->IDCMPWindow, TRUE);
      //       break;
      //   }

      //   GT_ReplyIMsg(pMsg);
      // }
    }

    if (flags & requestWindowFlags)
    {
      selected = SysReqHandler(pRequesterWindow, NULL, FALSE);
    }

  }
  while (selected < 0);

  FreeSysRequest(pRequesterWindow);

  // Unblock the window
  EndRequest(&sleepRequester, pWindow);
  SetWindowPointer(pWindow, WA_BusyPointer, FALSE, TAG_DONE);

  return selected;
}

struct FileRequester* showMultiFileSelector(struct Window* pParentWindow,
                                            STRPTR pHeaderText,
                                            struct Hook* pIntuiMsgHook)
{
  struct Requester sleepRequester;
  struct FileRequester* pFileRequest;


  // Allocate data structure for the ASL requester
  if(!(pFileRequest = (struct FileRequester*)
    AllocAslRequestTags(ASL_FileRequest,
                        ASLFR_TitleText, (ULONG) pHeaderText,
                        // ASLFR_InitialDrawer, (ULONG) initialPath.c_str(),
                        // ASLFR_InitialFile, (ULONG) initialFile.c_str(),
                        ASLFR_Window, (ULONG) pParentWindow,
                        ASLFR_RejectIcons, TRUE,
                        ASLFR_DoMultiSelect, TRUE,
                        ASLFR_IntuiMsgFunc, (ULONG)pIntuiMsgHook,
                        TAG_DONE)))
  {
    // Data struct allocation failed
    return NULL;
  }

  // Block the window that this requester is tied to
  InitRequester(&sleepRequester);
  Request(&sleepRequester, pParentWindow);
  SetWindowPointer(pParentWindow, WA_BusyPointer, TRUE, TAG_DONE);

  // Open the file requester and wait until the user selected a file
  if(AslRequestTags(pFileRequest, TAG_DONE) == FALSE)
  {
    FreeAslRequest((APTR)pFileRequest);
    pFileRequest = NULL;
  }

  // Unblock the window
  EndRequest(&sleepRequester, pParentWindow);
  SetWindowPointer(pParentWindow, WA_BusyPointer, FALSE, TAG_DONE);

  return pFileRequest;
}

void freeMultiFileSelector(struct FileRequester* pFileRequester)
{
  if(!pFileRequester)
  {
    return;
  }

  FreeAslRequest((APTR)pFileRequester);
}
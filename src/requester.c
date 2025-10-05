#include <classes/window.h>
#include <libraries/asl.h>

// clang-format off
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
// clang-format on

#include "requester.h"

long showEasyRequest(Object *pWinObject,
  struct Window *pWindow,
  char *pTitle,
  char *pButtonTexts,
  char *pMessage)
{
  ULONG requestWindowFlags = 0, activeWindowFlags = 0;
  struct Window *pRequesterWindow;
  struct Requester sleepRequester;
  ULONG code, result;
  long selected = -1;

  //
  // Instead of just calling `EasyRequestArgs(...) the EasyRequest is
  // built manually. This low-level approach allows better control of
  // the requests behavior. For example it gives access to the
  // requesters event loop where Intuition messages can be received. In
  // this way it can be detected if e.g. the requester parent window has
  // been resized; and it can be repainted.
  //
  struct EasyStruct easyStruct = {
    sizeof(struct EasyStruct),
    0,
  };

  easyStruct.es_Title = pTitle;
  easyStruct.es_TextFormat = pMessage;
  easyStruct.es_GadgetFormat = pButtonTexts;

  if (!(pRequesterWindow = BuildEasyRequestArgs(pWindow, &easyStruct, 0, NULL)))
  {
    return -1;
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
    }

    if (flags & requestWindowFlags)
    {
      selected = SysReqHandler(pRequesterWindow, NULL, FALSE);
    }

  } while (selected < 0);

  FreeSysRequest(pRequesterWindow);

  // Unblock the window
  EndRequest(&sleepRequester, pWindow);
  SetWindowPointer(pWindow, WA_BusyPointer, FALSE, TAG_DONE);

  return selected;
}

void __ASM__ __SAVE_DS__ IntuiMsgFunc(__REG__(a0, struct Hook *pHook),
  __REG__(a2, struct FileRequester *pRequester),
  __REG__(a1, struct IntuiMessage *pMsg))
{
  Object *pWinObject = (Object *)pHook->h_Data;

  switch (pMsg->Class)
  {
  // One of the windows has been resized
  case IDCMP_NEWSIZE:
  {
    DoMethod(pWinObject, WM_RETHINK);
    break;
  }
  }
}

struct Hook m_IntuiMsgHook;

struct FileRequester *showMultiFileSelector(Object *pWinObject,
  struct Window *pParentWindow,
  STRPTR pTitle,
  STRPTR pInitialDirectory)
{
  struct Requester sleepRequester;
  struct FileRequester *pFileRequest;

  m_IntuiMsgHook.h_Entry = (ULONG (*)())IntuiMsgFunc;
  m_IntuiMsgHook.h_SubEntry = NULL;
  m_IntuiMsgHook.h_Data = pWinObject;

  // Allocate data structure for the ASL requester

  // clang-format off
  pFileRequest = (struct FileRequester*) AllocAslRequestTags(ASL_FileRequest,
    ASLFR_TitleText, (ULONG) pTitle,
    ASLFR_InitialDrawer, (ULONG) pInitialDirectory,
    // ASLFR_InitialFile, (ULONG) initialFile.c_str(),
    ASLFR_Window, (ULONG) pParentWindow,
    ASLFR_RejectIcons, TRUE,
    ASLFR_DoMultiSelect, TRUE,
    ASLFR_IntuiMsgFunc, (ULONG)&m_IntuiMsgHook,
    TAG_DONE);
  // clang-format on

  if (!pFileRequest)
  {
    // Data struct allocation failed
    return NULL;
  }

  // Block the window that this requester is tied to
  InitRequester(&sleepRequester);
  Request(&sleepRequester, pParentWindow);
  SetWindowPointer(pParentWindow, WA_BusyPointer, TRUE, TAG_DONE);

  // Open the file requester and wait until the user selected a file
  if (AslRequestTags(pFileRequest, TAG_DONE) == FALSE)
  {
    FreeAslRequest((APTR)pFileRequest);
    pFileRequest = NULL;
  }

  // Unblock the window
  EndRequest(&sleepRequester, pParentWindow);
  SetWindowPointer(pParentWindow, WA_BusyPointer, FALSE, TAG_DONE);

  return pFileRequest;
}

void freeMultiFileSelector(struct FileRequester *pFileRequester)
{
  if (!pFileRequester)
  {
    return;
  }

  FreeAslRequest((APTR)pFileRequester);
}
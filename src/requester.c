#include <libraries/asl.h>

#ifdef __clang__
#include <clib/asl_protos.h>
  #include <clib/intuition_protos.h>
#else
  #include <proto/asl.h>
  #include <proto/intuition.h>
#endif

#include "requester.h"

long showEasyRequest(struct Window* pWindow,
                     char* pButtonTexts,
                     char* pMessage)
{
  struct Requester sleepRequester;
  long result;
  struct EasyStruct easyStruct =
  {
    sizeof(struct EasyStruct),
    0,
    "MultiRename",
    NULL,
    NULL
  };

  easyStruct.es_TextFormat = pMessage;
  easyStruct.es_GadgetFormat = pButtonTexts;

  // Block the window that this requester is tied to
  InitRequester(&sleepRequester);
  Request(&sleepRequester, pWindow);
  SetWindowPointer(pWindow, WA_BusyPointer, TRUE, TAG_DONE);

  // Show the requester ("message box")
  result = EasyRequestArgs(pWindow, &easyStruct, NULL, "");

  // Unblock the window
  EndRequest(&sleepRequester, pWindow);
  SetWindowPointer(pWindow, WA_BusyPointer, FALSE, TAG_DONE);

  return result;
}


struct FileRequester* showMultiFileSelector(struct Window* pParentWindow,
                                            STRPTR pHeaderText)
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
                        // ASLFR_IntuiMsgFunc, (ULONG)&aslHook,
                        TAG_DONE)))
  {
    // Data struct allocation failed
    return;
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
#ifdef __clang__
  #include <clib/intuition_protos.h>
#else
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

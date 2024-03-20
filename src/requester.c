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

  result = EasyRequestArgs(pWindow, &easyStruct, NULL, "");
  return result;
}

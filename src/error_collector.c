#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/dos_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
#endif


#include "error_collector.h"


ErrorCollector* createErrorCollector(void)
{
  ErrorCollector* pErrorCollector;
  if((pErrorCollector = AllocVec(sizeof(ErrorCollector), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    if((pErrorCollector->pErrorList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
    {
      NewList(pErrorCollector->pErrorList);
      return pErrorCollector;
    }
    else
    {
      freeErrorCollector(pErrorCollector);
    }

    return pErrorCollector;
  }
  
  return NULL;
}

void freeErrorCollector(ErrorCollector* pErrorCollector)
{
  if(!pErrorCollector)
  {
    return;
  }

  if(pErrorCollector->pErrorList)
  {
    clearErrors(pErrorCollector);

    FreeVec(pErrorCollector->pErrorList);
    pErrorCollector->pErrorList = NULL;
  }

  FreeVec(pErrorCollector);
}

void clearErrors(ErrorCollector* pErrorCollector)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pErrorCollector || !pErrorCollector->pErrorList)
  {
    return;
  }

  pWorkNode = pErrorCollector->pErrorList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    FreeVec(pWorkNode);
    pWorkNode = pNextNode;
  }
}

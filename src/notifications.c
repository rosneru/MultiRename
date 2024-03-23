#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/dos_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
#endif


#include "notifications.h"


NotificationCollector* createNotificationCollector(void)
{
  NotificationCollector* pNotificationCollector;
  if((pNotificationCollector = AllocVec(sizeof(NotificationCollector), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    if((pNotificationCollector->pErrorList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
    {
      NewList(pNotificationCollector->pErrorList);
      return pNotificationCollector;
    }
    else
    {
      freeNotificationCollector(pNotificationCollector);
    }

    return pNotificationCollector;
  }
  
  return NULL;
}

void freeNotificationCollector(NotificationCollector* pNotificationCollector)
{
  if(!pNotificationCollector)
  {
    return;
  }

  if(pNotificationCollector->pErrorList)
  {
    clearNotifications(pNotificationCollector);

    FreeVec(pNotificationCollector->pErrorList);
    pNotificationCollector->pErrorList = NULL;
  }

  FreeVec(pNotificationCollector);
}

void clearNotifications(NotificationCollector* pNotificationCollector)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pNotificationCollector || !pNotificationCollector->pErrorList)
  {
    return;
  }

  pWorkNode = pNotificationCollector->pErrorList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    FreeVec(pWorkNode);
    pWorkNode = pNextNode;
  }
}

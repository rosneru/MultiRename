#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/dos_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
#endif

#include <stdio.h>
#include <string.h>

#include "notifications.h"


NotificationCollector* createNotificationCollector(void)
{
  NotificationCollector* pCollector;
  if((pCollector = AllocVec(sizeof(NotificationCollector),
                                        MEMF_PUBLIC|MEMF_CLEAR)))
  {
    if((pCollector->pErrorList = AllocVec(sizeof(struct List),
                                                      MEMF_PUBLIC|MEMF_CLEAR)))
    {
      NewList(pCollector->pErrorList);
      return pCollector;
    }
    else
    {
      freeNotificationCollector(pCollector);
    }

    return pCollector;
  }
  
  return NULL;
}

void freeNotificationCollector(NotificationCollector* pCollector)
{
  if(!pCollector)
  {
    return;
  }

  if(pCollector->pErrorList)
  {
    clearNotifications(pCollector);

    FreeVec(pCollector->pErrorList);
    pCollector->pErrorList = NULL;
  }

  FreeVec(pCollector);
}

void addNotification(NotificationCollector* pCollector,
                     NotificationNodeType type,
                     STRPTR pItemText)
{
  NotificationNode* pNode;
  ULONG textNumChars;

  if(!pItemText)
  {
    return;
  }

  if((pNode = AllocVec(sizeof(NotificationNode), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    textNumChars = strlen(pItemText);
    if((pNode->pItemText = AllocVec(textNumChars + 1, MEMF_PUBLIC)))
    {
      strcpy(pNode->pItemText, pItemText);
      pNode->Type = type;

      AddTail(pCollector->pErrorList, (struct Node*)pNode);
    }
  }
}

void clearNotifications(NotificationCollector* pCollector)
{
  NotificationNode* pNotificationNode;
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pCollector || !pCollector->pErrorList)
  {
    return;
  }

  pWorkNode = pCollector->pErrorList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    pNotificationNode = (NotificationNode*)pWorkNode;
    if(pNotificationNode->pItemText)
    {
      FreeVec(pNotificationNode->pItemText);
    }

    FreeVec(pWorkNode);
    pWorkNode = pNextNode;
  }
}

void printNotifications(NotificationCollector* pCollector)
{
  ULONG count;
  struct Node* pWorkNode;
  struct Node* pNextNode;
  NotificationNode* pNotificationNode;


  if((pNotificationNode = findFirstNotificationByType(pCollector,
                                                      NNT_SELECTED_PATH_INFO)))
  {
    printf("Selected path is '%s'\n\n", pNotificationNode->pItemText);
  }

  if(0 < (count = getNotificationCountByType(pCollector,
                                             NNT_SKIPPED_WRONG_PATH)))
  {
    printf("Skipped %u input file(s) because of wrong paths:\n", count);

    pWorkNode = pCollector->pErrorList->lh_Head;
    while((pNextNode = pWorkNode->ln_Succ))
    {
      pNotificationNode = (NotificationNode*)pWorkNode;
      if(pNotificationNode->Type == NNT_SKIPPED_WRONG_PATH)
      {
        printf("  %s\n", pNotificationNode->pItemText);
      }
    }

    printf("\n");
  }

  if(0 < (count = getNotificationCountByType(pCollector,
                                             NNT_SKIPPED_PATH_TOO_LONG)))
  {
    printf("Skipped %u input file(s) because of over long / "
           "truncated paths:\n", count);

    pWorkNode = pCollector->pErrorList->lh_Head;
    while((pNextNode = pWorkNode->ln_Succ))
    {
      pNotificationNode = (NotificationNode*)pWorkNode;
      if(pNotificationNode->Type == NNT_SKIPPED_PATH_TOO_LONG)
      {
        printf("  %s\n", pNotificationNode->pItemText);
      }
    }

    printf("\n");
  }
}

NotificationNode* findFirstNotificationByType(
  NotificationCollector* pCollector, NotificationNodeType type)
{
  NotificationNode* pNotificationNode;
  struct Node* pWorkNode;
  struct Node* pNextNode;

  pWorkNode = pCollector->pErrorList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    pNotificationNode = (NotificationNode*)pWorkNode;
    if(pNotificationNode->Type == type)
    {
      return pNotificationNode;
    }
  }

  return NULL;
}

ULONG getNotificationCountByType(NotificationCollector* pCollector,
                                 NotificationNodeType type)
{
  ULONG count = 0;
  NotificationNode* pNotificationNode;
  struct Node* pWorkNode;
  struct Node* pNextNode;

  pWorkNode = pCollector->pErrorList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    pNotificationNode = (NotificationNode*)pWorkNode;
    if(pNotificationNode->Type == type)
    {
      count++;
    }
  }

  return count;
}

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


struct List* createNotificationList(void)
{
  struct List* pList;
  if(!(pList = AllocVec(sizeof(struct List), MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pList);
  return pList;
}

void freeNotificationList(struct List* pList)
{
  if(!pList)
  {
    return;
  }

  clearNotifications(pList);
  FreeVec(pList);
}

void addNotification(struct List* pList,
                     NotificationNodeType type,
                     STRPTR pItemText)
{
  NotificationNode* pNode;
  ULONG textNumChars;

  if(!pList || !pItemText)
  {
    return;
  }

  if((pNode = AllocVec(sizeof(NotificationNode), MEMF_CLEAR)))
  {
    textNumChars = strlen(pItemText);
    if((pNode->pItemText = AllocVec(textNumChars + 1, MEMF_CLEAR)))
    {
      strcpy(pNode->pItemText, pItemText);
      pNode->Type = type;

      AddTail(pList, (struct Node*)pNode);
    }
  }
}

void clearNotifications(struct List* pList)
{
  NotificationNode* pNotificationNode;
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pList)
  {
    return;
  }

  pWorkNode = pList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    pNotificationNode = (NotificationNode*)pWorkNode;
    if(pNotificationNode->pItemText)
    {
      FreeVec(pNotificationNode->pItemText);
    }

    Remove(pWorkNode);

    FreeVec(pWorkNode);
    pWorkNode = pNextNode;
  }
}

void printNotifications(struct List* pList)
{
  ULONG count;
  struct Node* pNode;
  NotificationNode* pNotificationNode;


  if((pNotificationNode = findFirstNotificationByType(pList,
                                                      NNT_SELECTED_PATH_INFO)))
  {
    printf("Selected path is '%s'\n\n", pNotificationNode->pItemText);
  }

  if(0 < (count = getNotificationCountByType(pList,
                                             NNT_SKIPPED_WRONG_PATH)))
  {
    printf("Skipped %u input file(s) because of wrong paths:\n", count);

    for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      pNotificationNode = (NotificationNode*)pNode;
      if(pNotificationNode->Type == NNT_SKIPPED_WRONG_PATH)
      {
        printf("  %s\n", pNotificationNode->pItemText);
      }
    }

    printf("\n");
  }

  if(0 < (count = getNotificationCountByType(pList,
                                             NNT_SKIPPED_PATH_TOO_LONG)))
  {
    printf("Skipped %u input file(s) because of over long / "
           "truncated paths:\n", count);

    for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      pNotificationNode = (NotificationNode*)pNode;
      if(pNotificationNode->Type == NNT_SKIPPED_PATH_TOO_LONG)
      {
        printf("  %s\n", pNotificationNode->pItemText);
      }
    }

    printf("\n");
  }
}

NotificationNode* findFirstNotificationByType(struct List* pList,
                                              NotificationNodeType type)
{
  NotificationNode* pNotificationNode;
  struct Node* pNode;

  for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pNotificationNode = (NotificationNode*)pNode;
    if(pNotificationNode->Type == type)
    {
      return pNotificationNode;
    }
  }

  return NULL;
}

ULONG getNotificationCountByType(struct List* pList,
                                 NotificationNodeType type)
{
  ULONG count = 0;
  NotificationNode* pNotificationNode;
  struct Node* pNode;

  for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pNotificationNode = (NotificationNode*)pNode;
    if(pNotificationNode->Type == type)
    {
      count++;
    }
  }

  return count;
}

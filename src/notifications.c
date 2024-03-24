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
  struct Node* pNode;
  ULONG textNumChars;

  if(!pList || !pItemText)
  {
    return;
  }

  if((pNode = AllocVec(sizeof(struct Node), MEMF_CLEAR)))
  {
    textNumChars = strlen(pItemText);
    if((pNode->ln_Name = AllocVec(textNumChars + 1, MEMF_CLEAR)))
    {
      strcpy(pNode->ln_Name, pItemText);
      pNode->ln_Type = type;

      AddTail(pList, (struct Node*)pNode);
    }
  }
}

void clearNotifications(struct List* pList)
{
  clearNotificationsExcept(pList, NNT_NONE);
}

void clearNotificationsExcept(struct List* pList, NotificationNodeType exceptType)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pList)
  {
    return;
  }

  pWorkNode = pList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    if(exceptType != NNT_NONE)
    {
      if(pWorkNode->ln_Type == exceptType)
      {
        // Skip / don't delete this node
        pWorkNode = pNextNode;
        continue;
      }
    }

    if(pWorkNode->ln_Name)
    {
      FreeVec(pWorkNode->ln_Name);
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

  if((pNode = findFirstNotificationByType(pList, NNT_SELECTED_PATH_INFO)))
  {
    printf("Selected path is '%s'\n\n", pNode->ln_Name);
  }

  if(0 < (count = getNotificationCountByType(pList, NNT_SKIPPED_WRONG_PATH)))
  {
    printf("Skipped %u input file(s) because of wrong paths:\n", count);

    for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if(pNode->ln_Type == NNT_SKIPPED_WRONG_PATH)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if(0 < (count = getNotificationCountByType(pList, NNT_SKIPPED_PATH_TOO_LONG)))
  {
    printf("Skipped %u input file(s) because of over long / "
           "truncated paths:\n", count);

    for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if(pNode->ln_Type == NNT_SKIPPED_PATH_TOO_LONG)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }
}

BOOL containsSkippedNotifications(struct List* pList)
{
  if(0 < getNotificationCountByType(pList, NNT_SKIPPED_PATH_TOO_LONG))
  {
    return TRUE;
  }

  if(0 < getNotificationCountByType(pList, NNT_SKIPPED_WRONG_PATH))
  {
    return TRUE;
  }

  return FALSE;
}

struct Node* findFirstNotificationByType(struct List* pList,
                                         NotificationNodeType type)
{
  struct Node* pNode;

  for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    if(pNode->ln_Type == type)
    {
      return pNode;
    }
  }

  return NULL;
}

ULONG getNotificationCountByType(struct List* pList,
                                 NotificationNodeType type)
{
  ULONG count = 0;
  struct Node* pNotificationNode;
  struct Node* pNode;

  for(pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pNotificationNode = (struct Node*)pNode;
    if(pNotificationNode->ln_Type == type)
    {
      count++;
    }
  }

  return count;
}

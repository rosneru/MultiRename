// clang-format off
#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
#endif
// clang-format on

#include <stdio.h>
#include <string.h>

#include "notifications.h"

#define CATCOMP_NUMBERS
#include "multirename_catalog.h"


struct List *createNotificationList(void)
{
  struct List *pList;
  if (!(pList = AllocVec(sizeof(struct List), MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pList);
  return pList;
}

void freeNotificationList(struct List *pList)
{
  if (!pList)
  {
    return;
  }

  clearNotifications(pList);
  FreeVec(pList);
}

void addNotification(
  struct List *pList, NotificationNodeType type, STRPTR pItemText)
{
  struct Node *pNode;
  ULONG textNumChars;

  if (!pList || !pItemText)
  {
    return;
  }

  if ((pNode = AllocVec(sizeof(struct Node), MEMF_CLEAR)))
  {
    textNumChars = strlen(pItemText);
    if ((pNode->ln_Name = AllocVec(textNumChars + 1, MEMF_CLEAR)))
    {
      strcpy(pNode->ln_Name, pItemText);
      pNode->ln_Type = type;

      AddTail(pList, (struct Node *)pNode);
    }
  }
}

void clearNotifications(struct List *pList)
{
  clearNotificationsExcept(pList, NNT_NONE);
}

void clearNotificationsExcept(
  struct List *pList, NotificationNodeType exceptType)
{
  struct Node *pWorkNode;
  struct Node *pNextNode;

  if (!pList)
  {
    return;
  }

  pWorkNode = pList->lh_Head;
  while ((pNextNode = pWorkNode->ln_Succ))
  {
    if (exceptType != NNT_NONE)
    {
      if (pWorkNode->ln_Type == exceptType)
      {
        // Skip / don't delete this node
        pWorkNode = pNextNode;
        continue;
      }
    }

    if (pWorkNode->ln_Name)
    {
      FreeVec(pWorkNode->ln_Name);
    }

    Remove(pWorkNode);
    FreeVec(pWorkNode);
    pWorkNode = pNextNode;
  }
}

void printNotifications(struct List *pList, struct LocaleInfo *pLocaleInfo)
{
  ULONG count;
  struct Node *pNode;

  if ((pNode = findFirstNotificationByType(pList, NNT_SELECTED_PATH_INFO)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_SELECTED_PATH), pNode->ln_Name);
  }


  if (0 < (count = getNotificationCountByType(pList, NNT_SKIPPED_WRONG_PATH)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_WRONG_PATHS), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_WRONG_PATH)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 <
    (count = getNotificationCountByType(pList, NNT_SKIPPED_PATH_TOO_LONG)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_OVERLONG_PATHS), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_PATH_TOO_LONG)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 < (count = getNotificationCountByType(
             pList, NNT_SKIPPED_FAILED_DATETIMEPARTS)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_DATESTAMP), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_FAILED_LOCK)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 <
    (count = getNotificationCountByType(pList, NNT_SKIPPED_FAILED_EXAMINE)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_EXAMINE), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_FAILED_LOCK)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 < (count = getNotificationCountByType(pList, NNT_SKIPPED_FAILED_LOCK)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_LOCK), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_FAILED_LOCK)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 < (count = getNotificationCountByType(
             pList, NNT_SKIPPED_LINKS_NOT_SUPPORTED)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_LINKS), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_LINKS_NOT_SUPPORTED)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 < (count = getNotificationCountByType(pList, NNT_SKIPPED_DUPLICATE)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_ALREADY_IN_LST), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_SKIPPED_DUPLICATE)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }

  if (0 < (count = getNotificationCountByType(pList, NNT_RENAME_FAILED)))
  {
    printf(tr(pLocaleInfo, MSG_LOG_RENAME_FAILED), count);

    for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
    {
      if (pNode->ln_Type == NNT_RENAME_FAILED)
      {
        printf("  %s\n", pNode->ln_Name);
      }
    }

    printf("\n");
  }
}

BOOL containsSkippedNotifications(struct List *pList)
{
  if (0 < getNotificationCountByType(pList, NNT_SKIPPED_PATH_TOO_LONG))
  {
    return TRUE;
  }

  if (0 < getNotificationCountByType(pList, NNT_SKIPPED_WRONG_PATH))
  {
    return TRUE;
  }

  if (0 < getNotificationCountByType(pList, NNT_SKIPPED_FAILED_LOCK))
  {
    return TRUE;
  }

  if (0 < getNotificationCountByType(pList, NNT_SKIPPED_DUPLICATE))
  {
    return TRUE;
  }

  if (0 < getNotificationCountByType(pList, NNT_SKIPPED_LINKS_NOT_SUPPORTED))
  {
    return TRUE;
  }

  return FALSE;
}

struct Node *findFirstNotificationByType(
  struct List *pList, NotificationNodeType type)
{
  struct Node *pNode;

  for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    if (pNode->ln_Type == type)
    {
      return pNode;
    }
  }

  return NULL;
}

ULONG getNotificationCountByType(struct List *pList, NotificationNodeType type)
{
  ULONG count = 0;
  struct Node *pNotificationNode;
  struct Node *pNode;

  for (pNode = pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pNotificationNode = (struct Node *)pNode;
    if (pNotificationNode->ln_Type == type)
    {
      count++;
    }
  }

  return count;
}

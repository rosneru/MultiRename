#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <exec/lists.h>
#include <exec/types.h>

typedef enum NotificationNodeType
{
  NNT_SELECTED_PATH_INFO,
  NNT_SKIPPED_WRONG_PATH,
  NNT_SKIPPED_PATH_TOO_LONG
} NotificationNodeType;

typedef struct NotificationNode
{
  struct Node en_Node;
  NotificationNodeType Type;
  STRPTR pItemText;
} NotificationNode;


struct List* createNotificationList(void);
void freeNotificationList(struct List* pList);

void addNotification(struct List* pList,
                     NotificationNodeType type,
                     STRPTR pItemText);

void clearNotifications(struct List* pList);
void printNotifications(struct List* pList);

struct NotificationNode* findFirstNotificationByType(struct List* pList,
                                                     NotificationNodeType type);

ULONG getNotificationCountByType(struct List* pList,
                                 NotificationNodeType type);

#endif

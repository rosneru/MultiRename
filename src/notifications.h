#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

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

typedef struct NotificationCollector
{
  struct List* pErrorList;
} NotificationCollector;



NotificationCollector* createNotificationCollector(void);
void freeNotificationCollector(NotificationCollector* pCollector);

void addNotification(NotificationCollector* pCollector,
                     NotificationNodeType type,
                     STRPTR pItemText);

void clearNotifications(NotificationCollector* pCollector);
void printNotifications(NotificationCollector* pCollector);

struct NotificationNode* findFirstNotificationByType(
  NotificationCollector* pCollector, NotificationNodeType type);

ULONG getNotificationCountByType(NotificationCollector* pCollector,
                                 NotificationNodeType type);

#endif

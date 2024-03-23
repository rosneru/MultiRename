#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <exec/types.h>

typedef enum NotificationType
{
  NT_Skipped,
} NotificationType;

typedef struct NotificationNode
{
  struct Node en_Node;
  NotificationType NotificationType;
  char* pItemName;
} NotificationNode;

typedef struct NotificationCollector
{
  struct List* pErrorList;
} NotificationCollector;



NotificationCollector* createNotificationCollector(void);
void freeNotificationCollector(NotificationCollector* pNotificationCollector);

void clearNotifications(NotificationCollector* pNotificationCollector);


#endif

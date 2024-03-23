#ifndef ERROR_COLLECTOR_H
#define ERROR_COLLECTOR_H

#include <exec/types.h>

typedef enum ErrorType
{
  ET_Skipped,
} ErrorType;

typedef struct ErrorNode
{
  struct Node en_Node;
  ErrorType ErrorType;
  char pItemName;
} ErrorNode;

typedef struct ErrorCollector
{
  struct List* pErrorList;
} ErrorCollector;

ErrorCollector* createErrorCollector(void);
void freeErrorCollector(ErrorCollector* pErrorCollector);

void clearErrors(ErrorCollector* pErrorCollector);


#endif
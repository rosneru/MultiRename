#include "lists.h"

void NewList(struct List* pList)
{
  if(!pList)
  {
    return;
  }

  pList->lh_Head = (struct Node*)&pList->lh_Tail;
  pList->lh_Tail = NULL;
  pList->lh_TailPred = (struct Node*)&pList->lh_Head;
}


void AddTail(struct List *pList, struct Node *pNode)
{
  pNode->ln_Succ                = (struct Node *)&pList->lh_Tail;
  pNode->ln_Pred                = pList->lh_TailPred;
  pList->lh_TailPred->ln_Succ   = pNode;
  pList->lh_TailPred            = pNode;
}

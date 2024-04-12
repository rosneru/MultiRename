#ifndef AMIGA_EXEC_LISTS_H
#define AMIGA_EXEC_LISTS_H

#include <exec/types.h>
#include <exec/nodes.h>


struct List
{
  struct Node* lh_Head;
  struct Node* lh_Tail;
  struct Node* lh_TailPred;
  UBYTE        lh_Type;
  UBYTE        lh_Pad;
};

#endif

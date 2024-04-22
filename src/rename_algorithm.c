#include <exec/types.h>

#include <stdlib.h>
#include <string.h>

#include "file_node.h"
#include "rename_parser.h"
#include "rename_algorithm.h"

/**
 * Fills the given pResultBuf with a new name which is constructed from
 * the current name pName, the rename mask pMask, the counter and the
 * list of rename actions that have been parsed from the pMask before.
 */
void applyActions(STRPTR pResultBuf,
                  struct List* pActionList,
                  STRPTR pName,
                  UBYTE NameLen,
                  STRPTR pMask,
                  Counter* pCounter)
{
  struct Node* pNode;
  ActionNode* pAction;
  BOOL mustIncrementCounter = FALSE;
  ULONG numChars;
  STRPTR pExt = pName + NameLen + 1;

  if(!pResultBuf || !pActionList || !pName || !pMask || !pCounter)
  {
    return;
  }

  strcpy(pResultBuf, "");

  for(pNode = pActionList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pAction = (ActionNode*)pNode;
    switch(pAction->Command)
    {
      case AC_APPLY:
      {
        numChars = pAction->End - pAction->Start + 1;
        strncat(pResultBuf, pMask + pAction->Start, numChars);
        break;
      }
      case AC_COUNTER:
      {
        strcat(pResultBuf, getCounterValue(pCounter));
        mustIncrementCounter = TRUE;
        break;
      }
      case AC_NAME:
      {
        if((pAction->Start > -1) && (pAction->End > -1))
        {
          numChars = pAction->End - pAction->Start + 1;
          strncat(pResultBuf, pName + pAction->Start, numChars);
        }
        else
        {
          strncat(pResultBuf, pName, NameLen);
        }
        break;
      }
      case AC_EXTENSION:
      {
        if((pAction->Start > -1) && (pAction->End > -1))
        {
          numChars = pAction->End - pAction->Start + 1;
          strncat(pResultBuf, pExt + pAction->Start, numChars);
        }
        else
        {
          strncat(pResultBuf, pExt, NameLen);
        }
        break;
      }
      case AC_NONE:
      {
        // Nothing
        break;
      }
    }
  }

  if(mustIncrementCounter)
  {
    incrementCounter(pCounter);
  }
}


BOOL fillNewNames(struct List* pFilesList,
                  STRPTR pNameMask,
                  STRPTR pExtMask,
                  LONG counterStart,
                  LONG counterInc,
                  BYTE counterWidth)
{
  struct Node* pNode;
  FileNode* pFileNode;
  ULONG maskSize;
  STRPTR pMask;
  Counter counter;
  ActionParser parser;

  if(!pFilesList || ! pNameMask || ! pExtMask)
  {
    return FALSE;
  }

  maskSize = strlen(pNameMask) + strlen(pExtMask) + 2;
  if(!(pMask = malloc(maskSize * sizeof(char))))
  {
    return FALSE;
  }

  // Build the resulting mask of name and extension field to be used by
  // the parser.
  strcpy(pMask, pNameMask);
  strcat(pMask, ".");
  strcat(pMask, pExtMask);

  initCounter(&counter, counterStart, counterInc, counterWidth);
  initActionParser(&parser, pMask);

  if(!parseActions(&parser))
  {
    freeActionNodes(&parser.ActionList);
    free(pMask);
    return FALSE;
  }

  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    applyActions(pFileNode->NewName,
                 &parser.ActionList,
                 pFileNode->OldName,
                 pFileNode->OldNameLen,
                 pMask, &counter);
  }

  freeActionNodes(&parser.ActionList);
  free(pMask);
  return TRUE;
}


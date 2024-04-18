#include <exec/types.h>

#include <string.h>

#include "file_node.h"
#include "rename_parser.h"
#include "rename_algorithm.h"

/**
 * Fills the given pResultBuf with a new name which is constructed from
 * the current name pPart, the rename mask pMask, the counter and the
 * list of rename actions that have been parsed from the pMask before.
 */
void applyActions(STRPTR pResultBuf,
                  struct List* pActionList,
                  STRPTR pPart,
                  STRPTR pMask,
                  Counter* pCounter);


BOOL fillNewNames(struct List* pFilesList,
                  STRPTR pNameMask,
                  STRPTR pExtMask,
                  LONG counterStart,
                  LONG counterInc,
                  BYTE counterWidth)
{
  struct Node* pNode;
  FileNode* pFileNode;

  Counter nameCounter, extCounter;
  ActionParser nameParser, extParser;

  initCounter(&nameCounter, counterStart, counterInc, counterWidth);
  initCounter(&extCounter, counterStart, counterInc, counterWidth);

  initActionParser(&nameParser, pNameMask);
  initActionParser(&extParser, pExtMask);

  if(!parseActions(&nameParser))
  {
    return FALSE;
  }

  if(!parseActions(&extParser))
  {
    return FALSE;
  }

  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    applyActions(pFileNode->NewName, &nameParser.ActionList, pFileNode->OldName, pNameMask, &nameCounter);
    applyActions(pFileNode->NewExt, &extParser.ActionList, pFileNode->OldExt, pExtMask, &extCounter);
  }

  return TRUE;
}



void applyActions(STRPTR pResultBuf,
                  struct List* pActionList,
                  STRPTR pPart,
                  STRPTR pMask,
                  Counter* pCounter)
{
  struct Node* pNode;
  ActionNode* pAction;
  BOOL mustIncrementCounter = FALSE;
  ULONG numChars;

  if(!pResultBuf || !pActionList || !pPart || !pMask || !pCounter)
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
      case AC_EXTENSION:
      {
        if((pAction->Start > -1) && (pAction->End > -1))
        {
          numChars = pAction->End - pAction->Start + 1;
          strncat(pResultBuf, pPart + pAction->Start, numChars);
        }
        else
        {
          strcat(pResultBuf, pPart);
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

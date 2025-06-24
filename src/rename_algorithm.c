#include <exec/types.h>

#include <stdlib.h>
#include <string.h>

#include "string_tools.h"
#include "file_node.h"
#include "rename_parser.h"
#include "rename_algorithm.h"


/// Forwards / private function declarations

/**
 * Fills the NewName field of `FileNode*` with the new name
 * that is computed from the OriginalName field, the rename mask pMask,
 * the counter and the list of rename actions (that must have been
 * parsed from the pMask in the former step)
 */
void applyActions(FileNode* pFileNode,
                  ULONG bufSize,
                  struct List* pActionList,
                  STRPTR pMask,
                  Counter* pCounter);

///
/// Public function implementations

BOOL createNewNames(FileNodes* pFiles,
                    ULONG maxAllowedNameLength,
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

  if(!pFiles || !pFiles->pList || ! pNameMask || ! pExtMask)
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

  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    applyActions(pFileNode,
                 maxAllowedNameLength + 1,  // buffer *is* one bigger than the
                 &parser.ActionList,        // max name len for the trailing '\0'
                 pMask,
                 &counter);

    // Create a token for the new name that will be used for duplicate
    // detection in the first pass before the actual renaming starts.
    pFileNode->NewNameToken = createStringToken(pFileNode->NewName,
                                                pFileNode->NewNameFullLen);
  }

  freeActionNodes(&parser.ActionList);
  free(pMask);
  return TRUE;
}

///
/// Private function implementations

void applyActions(FileNode* pFileNode,
                  ULONG bufSize,
                  struct List* pActionList,
                  STRPTR pMask,
                  Counter* pCounter)
{
  struct Node* pNode;
  ActionNode* pAction;
  BOOL mustIncrementCounter = FALSE;
  ULONG numChars, end;
  int lastIndex;
  STRPTR pExt = pFileNode->OriginalName + pFileNode->OriginalNameLen + 1;

  if(!pFileNode || !pActionList || !pMask || !pCounter)
  {
    return;
  }

  strcpy(pFileNode->NewName, "");
  pFileNode->IsNewNameTruncated = FALSE;

  for(pNode = pActionList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pAction = (ActionNode*)pNode;
    switch(pAction->Command)
    {
      case AC_APPLY:
      {
        numChars = pAction->End - pAction->Start + 1;
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pMask + pAction->Start,
                                                      numChars);
        break;
      }
      case AC_COUNTER:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      getCounterValue(pCounter),
                                                      0);
        mustIncrementCounter = TRUE;
        break;
      }
      case AC_NAME:
      {
        if((pAction->Start > -1) && (pAction->End > -1))
        {
          end = pAction->End;
          if(end >= pFileNode->OriginalNameLen)
          {
            end = pFileNode->OriginalNameLen - 1;
          }

          numChars = end - pAction->Start + 1;
          pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                        bufSize,
                                                        &pFileNode->NewNameFullLen,
                                                        pFileNode->OriginalName + pAction->Start,
                                                        numChars);
        }
        else
        {
          pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                        bufSize,
                                                        &pFileNode->NewNameFullLen,
                                                        pFileNode->OriginalName,
                                                        pFileNode->OriginalNameLen);
        }
        break;
      }
      case AC_EXTENSION:
      {
        if((pAction->Start > -1) && (pAction->End > -1))
        {
          end = pAction->End;
          if(end >= pFileNode->OriginalExtLen)
          {
            end = pFileNode->OriginalExtLen - 1;
          }

          numChars = end - pAction->Start + 1;
          pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                        bufSize,
                                                        &pFileNode->NewNameFullLen,
                                                        pExt + pAction->Start,
                                                        numChars);
        }
        else
        {
          pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                        bufSize,
                                                        &pFileNode->NewNameFullLen,
                                                        pExt,
                                                        pFileNode->OriginalExtLen);
        }
        break;
      }
      case AC_YEAR:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pFileNode->OriginalDate.pYear,
                                                      4);
        break;
      }
      case AC_MONTH:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pFileNode->OriginalDate.pMonth,
                                                      2);
        break;
      }
      case AC_DAY:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pFileNode->OriginalDate.pDay,
                                                      2);
        break;
      }
      case AC_HOUR:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pFileNode->OriginalDate.pHour,
                                                      2);
        break;
      }
      case AC_MINUTE:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pFileNode->OriginalDate.pMinute,
                                                      2);
        break;
      }
      case AC_SECOND:
      {
        pFileNode->IsNewNameTruncated |= appendString(pFileNode->NewName,
                                                      bufSize,
                                                      &pFileNode->NewNameFullLen,
                                                      pFileNode->OriginalDate.pSecond,
                                                      2);
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

  // Remove trailing '.'
  lastIndex = strlen(pFileNode->NewName);
  if(lastIndex == 0)
  {
    return;
  }
  
  while(pFileNode->NewName[--lastIndex] == '.')
  {
    pFileNode->NewName[lastIndex] = '\0';
  }

}

///

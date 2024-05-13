#include <exec/types.h>

#include <stdlib.h>
#include <string.h>

#include "file_node.h"
#include "rename_parser.h"
#include "rename_algorithm.h"


/// Private function forward declarations


/**
 * Fills the given NewName field of given FileNode* with the new name
 * that is computed from the OriginalName field, the rename mask pMask,
 * the counter and the list of rename actions (that must have been
 * parsed from the pMask in the former step)
 */
void applyActions(FileNode* pFileNode,
                  ULONG bufSize,
                  struct List* pActionList,
                  STRPTR pMask,
                  Counter* pCounter);

/**
 * This own 'strcat' implementation takes care of the destination buffer
 * size and doesn't write beyond the destination buffer borders. Instead
 * it truncates if necessary. It tries to copy numChars from src to
 * destination buf. Set numChars to 0 to copy the complete src string.
 *
 * It returns TRUE if no truncation was done and FALSE otherwise.
 */
BOOL appendString(STRPTR pDest, ULONG destSize, STRPTR pSrc, ULONG numChars);


/// Public function implementations


BOOL createNewNames(struct List* pFilesList,
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
    applyActions(pFileNode,
                 MAXNAMELEN + 1,      // buffer *is* one bigger than the
                 &parser.ActionList,  // max name len for the trailing '\0'
                 pMask,
                 &counter);
  }

  freeActionNodes(&parser.ActionList);
  free(pMask);
  return TRUE;
}


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

  for(pNode = pActionList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pAction = (ActionNode*)pNode;
    switch(pAction->Command)
    {
      case AC_APPLY:
      {
        numChars = pAction->End - pAction->Start + 1;
        appendString(pFileNode->NewName, bufSize, pMask + pAction->Start, numChars);
        break;
      }
      case AC_COUNTER:
      {
        appendString(pFileNode->NewName, bufSize, getCounterValue(pCounter), 0);
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
          appendString(pFileNode->NewName, bufSize, pFileNode->OriginalName + pAction->Start, numChars);
        }
        else
        {
          appendString(pFileNode->NewName, bufSize, pFileNode->OriginalName, pFileNode->OriginalNameLen);
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
          appendString(pFileNode->NewName, bufSize, pExt + pAction->Start, numChars);
        }
        else
        {
          appendString(pFileNode->NewName, bufSize, pExt, pFileNode->OriginalExtLen);
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

BOOL appendString(STRPTR pDest, ULONG destSize, STRPTR pSrc, ULONG numChars)
{
  ULONG remainingDestSize, srcLength, currentDestLength;
  
  currentDestLength = strlen(pDest);
  if(currentDestLength >= (destSize - 1))
  {
    return FALSE;
  }

  remainingDestSize = destSize - currentDestLength - 1;

  srcLength = strlen(pSrc);
  if(numChars > 0)
  {
    /* TODO Is here a min/max needed? */
    srcLength = numChars;
  }

  if(srcLength > remainingDestSize)
  {
    memcpy(pDest + currentDestLength, pSrc, remainingDestSize);
    pDest[destSize - 1] = '\0';
    return FALSE;
  }
  else
  {
    memcpy(pDest + currentDestLength, pSrc, srcLength);
    pDest[currentDestLength + srcLength] = '\0';
    return TRUE;
  }
}

#ifdef __clang__
  #include <clib/exec_protos.h>
#else
  #include <proto/exec.h>
#endif

#include "rename.h"

/// Forwards / private function declarations

TokenCount *appendTokenCount(ULONG appendToken,
                             TokenCount* pTokenCounts,
                             ULONG numTokenCounts);
TokenCount *findTokenCount(ULONG searchToken,
                           TokenCount* pTokenCounts,
                           ULONG numTokenCounts);

///
/// Public function implementations

BOOL renameFiles(struct List* pFilesList, struct List* pNotifications)
{
  struct Node* pNode;
  FileNode* pFileNode;

  if (!pFilesList || !pNotifications)
  {
    return FALSE;
  }

  // TODO
  // 1. How to get th lock of files directory?
  // 2. Change dir to files directory
  // 3. Call Rename() in loop below.
  // 3.1. Add errors to notifications

  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    printf("%s ==> %s\n", pFileNode->OriginalName, pFileNode->NewName);
  }

  return TRUE;
}


TokenCount* createTokenCounts(ULONG fileCount)
{
  TokenCount* pTokenCounts;

  if(fileCount < 1)
  {
    return NULL;
  }

  if(!(pTokenCounts = AllocVec(fileCount * sizeof(TokenCount), MEMF_CLEAR)))
  {
    return NULL;
  }

  return pTokenCounts;
}


void freeTokenCounts(TokenCount* pTokenCounts)
{
  if(!pTokenCounts)
  {
    return;
  }

  FreeVec(pTokenCounts);
}

void fillTokenOccurrences(struct List* pFilesList,
                          TokenCount* pTokenCounts,
                          ULONG numTokenCounts)
{
  ULONG i;
  struct Node* pNode;
  FileNode* pFileNode;
  TokenCount* pTokenCount;

  if (!pFilesList || !pTokenCounts || numTokenCounts < 1)
  {
    return;
  }

  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;

    if ((pTokenCount = findTokenCount(pFileNode->NewNameToken,
                                      pTokenCounts,
                                      numTokenCounts)))
    {
      pTokenCount->Count++;
    }
    else
    {
      if (!(pTokenCount = appendTokenCount(pFileNode->NewNameToken, pTokenCounts, numTokenCounts)))
      {
        printf("Failed to append token count for item '%10lu: %s'\n", pFileNode->NewNameToken,
                                                                      pFileNode->NewName);
        continue;
      }
    }

    pFileNode->TokenOccurrenceNumber = pTokenCount->Count;

  }
}

///
/// Private function implementations

TokenCount *findTokenCount(ULONG searchToken,
                           TokenCount* pTokenCounts,
                           ULONG numTokenCounts)
{
  ULONG i;
  for (i = 0; i < numTokenCounts; i++)
  {
    if (pTokenCounts[i].Token == searchToken)
    {
      return &pTokenCounts[i];
    }
  }

  return NULL;
}

TokenCount *appendTokenCount(ULONG appendToken,
                             TokenCount* pTokenCounts,
                             ULONG numTokenCounts)
{
  ULONG i;

  for (i = 0; i < numTokenCounts; i++)
  {
    if (pTokenCounts[i].Token == 0 && pTokenCounts[i].Count == 0)
    {
      pTokenCounts[i].Token = appendToken;
      pTokenCounts[i].Count = 1;
      return &pTokenCounts[i];
    }
  }

  return NULL;
}

///

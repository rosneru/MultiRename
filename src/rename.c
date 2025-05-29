#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/dos.h>
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
static BOOL alreadyRemoved = FALSE;

BOOL renameFiles(FileNodes* pFilesList,
                 struct List* pNotifications,
                 BOOL doSkipIcons,
                 STRPTR pTempBuf,
                 ULONG tempBufSize)
{
  struct Node* pNode;
  FileNode* pFileNode;
  BPTR pLock;

  if (!pFilesList || !pFilesList->pList || !pNotifications)
  {
    return FALSE;
  }

  // TODO
  // 1. How to get th lock of files directory?. DONE.
  // 2. Change dir to files directory
  Printf("Entering directory '%s'\n", pFilesList->DirPath);
  
  // 3. Call Rename() in loop below.
  // 3.1. Add errors to notifications
  for(pNode = pFilesList->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    Printf("RENAME '%s' ==> '%s'\n", pFileNode->OriginalName, pFileNode->NewName);


    // TODO: Continue
    // if(!doSkipIcons)
    // {
    //   pLock = Lock()
    // }

    if(!alreadyRemoved)
    {
      Remove(pNode);
      alreadyRemoved = TRUE;
    }
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

void fillTokenOccurrences(FileNodes* pFiles,
                          TokenCount* pTokenCounts,
                          ULONG numTokenCounts)
{
  struct Node* pNode;
  FileNode* pFileNode;
  TokenCount* pTokenCount;

  if (!pFiles || !pFiles->pList || !pTokenCounts || numTokenCounts < 1)
  {
    return;
  }

  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
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
        Printf("Failed to append token count for item '%10du: %s'\n", pFileNode->NewNameToken,
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

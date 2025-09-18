// clang-format off
#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/dos.h>
  #include <proto/exec.h>
#endif
// clang-format on

#include <string.h>

#include "rename.h"

/// Forwards / private function declarations

TokenCount *appendTokenCount(
  ULONG appendToken, TokenCount *pTokenCounts, ULONG numTokenCounts);
TokenCount *findTokenCount(
  ULONG searchToken, TokenCount *pTokenCounts, ULONG numTokenCounts);

///
/// Public function implementations
#define ERR_MSG_BUF_SIZE 512
BOOL renameFiles(
  FileNodes *pFilesList, struct List *pNotifications, BOOL doSkipIcons)
{
  struct Node *pNode;
  FileNode *pFileNode;
  BPTR pLock;
  BOOL wasCompletelySuccessful = TRUE;
  char oldIconName[128]; // Safe size, more than max file name length of 107
  char newIconName[128];
  char errMsgBuf[ERR_MSG_BUF_SIZE];

  if (!pFilesList || !pFilesList->pList || !pNotifications)
  {
    return FALSE;
  }

  for (pNode = pFilesList->pList->lh_Head; pNode->ln_Succ;
       pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode *)pNode;

    if (!doSkipIcons)
    {
      // Check if the file has an icon (.info file) NOTE: `oldIconName`
      // is bigger (PATH BUF SIZE) than a "file name" + ".info" ever can
      // be, so no size check is required
      strcpy(oldIconName, pFileNode->OriginalName);
      strcat(oldIconName, ".info");
      strcpy(newIconName, pFileNode->NewName);
      strcat(newIconName, ".info");
      if ((pLock = Lock(oldIconName, EXCLUSIVE_LOCK)))
      {
        UnLock(pLock);
        // Printf("  Found icon, renaming it: '%s' ==> '%s'\n", oldIconName,
        // newIconName);
        if (!Rename(oldIconName, newIconName))
        {
          Fault(IoErr(), newIconName, errMsgBuf, ERR_MSG_BUF_SIZE);
          addNotification(pNotifications, NNT_RENAME_FAILED, errMsgBuf);
          wasCompletelySuccessful = FALSE;
        }
      }
    }

    if (Rename(pFileNode->OriginalName, pFileNode->NewName))
    {
      Remove(pNode);
    }
    else
    {
      Fault(IoErr(), pFileNode->NewName, errMsgBuf, ERR_MSG_BUF_SIZE);
      addNotification(pNotifications, NNT_RENAME_FAILED, errMsgBuf);
      wasCompletelySuccessful = FALSE;
    }
  }

  return wasCompletelySuccessful;
}

TokenCount *createTokenCounts(ULONG fileCount)
{
  TokenCount *pTokenCounts;

  if (fileCount < 1)
  {
    return NULL;
  }

  if (!(pTokenCounts = AllocVec(fileCount * sizeof(TokenCount), MEMF_CLEAR)))
  {
    return NULL;
  }

  return pTokenCounts;
}

void freeTokenCounts(TokenCount *pTokenCounts)
{
  if (!pTokenCounts)
  {
    return;
  }

  FreeVec(pTokenCounts);
}

void fillTokenOccurrences(
  FileNodes *pFiles, TokenCount *pTokenCounts, ULONG numTokenCounts)
{
  struct Node *pNode;
  FileNode *pFileNode;
  TokenCount *pTokenCount;

  if (!pFiles || !pFiles->pList || !pTokenCounts || numTokenCounts < 1)
  {
    return;
  }

  for (pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode *)pNode;

    if ((pTokenCount = findTokenCount(
           pFileNode->NewNameToken, pTokenCounts, numTokenCounts)))
    {
      pTokenCount->Count++;
    }
    else
    {
      if (!(pTokenCount = appendTokenCount(
              pFileNode->NewNameToken, pTokenCounts, numTokenCounts)))
      {
        Printf("Failed to append token count for item '%10du: %s'\n",
          pFileNode->NewNameToken,
          pFileNode->NewName);
        continue;
      }
    }

    pFileNode->TokenOccurrenceNumber = pTokenCount->Count;
  }
}

///
/// Private function implementations

TokenCount *findTokenCount(
  ULONG searchToken, TokenCount *pTokenCounts, ULONG numTokenCounts)
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

TokenCount *appendTokenCount(
  ULONG appendToken, TokenCount *pTokenCounts, ULONG numTokenCounts)
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

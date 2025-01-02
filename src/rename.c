#ifdef __clang__
  #include <clib/exec_protos.h>
#else
  #include <proto/exec.h>
#endif

#include "rename.h"

TokenCount* createTokenCounts(ULONG fileCount)
{
  TokenCount* pTokenCounts;

  if(!(pTokenCounts = AllocVec(fileCount * sizeof(TokenCount), MEMF_CLEAR)))
  {
    return NULL;
  }
}


void freeTokenCounts(TokenCount* pTokenCounts)
{
  if(!pTokenCounts)
  {
    return NULL;
  }

  FreeVec(pTokenCounts);
}

// === BEGIN private ===
TokenCount *findTokenCount(ULONG searchToken,
                           ULONG itemCount)
{
  ULONG i;
  for (i = 0; i < itemCount; i++)
  {
    // if (tokenCounts[i].Token == searchToken)
    // {
    //   return &tokenCounts[i];
    // }
  }

  return NULL;
}

TokenCount *appendTokenCount(ULONG token,
                             ULONG itemCount)
{
  ULONG i;

  for (i = 0; i < itemCount; i++)
  {
    // if (tokenCounts[i].Token == 0 && tokenCounts[i].Count == 0)
    // {
    //   tokenCounts[i].Token = token;
    //   tokenCounts[i].Count = 1;
    //   return &tokenCounts[i];
    // }
  }

  return NULL;
}
// === END private ===

void parseTokenOccurrences(struct List *pFileList, ULONG itemCount)
{
  ULONG i;
  TokenCount* pTokenCount;

  // if (!pItems)
  // {
  //   return;
  // }

  // for (i = 0; i < itemCount; i++)
  // {
  //   if (pTokenCount = findTokenCount(items[i].Token, itemCount))
  //   {
  //     pTokenCount->Count++;
  //   }
  //   else
  //   {
  //     if (!(pTokenCount = appendTokenCount(items[i].Token, itemCount)))
  //     {
  //       printf("Failed to append token count for item '%10lu: %s'\n", items[i].Token,
  //                                                                     items[i].pText);
  //       continue;
  //     }
  //   }

    // items[i].OccurrenceNumber = pTokenCount->Count;
  // }
}

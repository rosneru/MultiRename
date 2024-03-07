#include <gadgets/chooser.h>
#include <gadgets/listbrowser.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/listbrowser_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
  #include <proto/listbrowser.h>
#endif

#include "file_node.h"

UBYTE* dummyCol1[] =
{
  "My_1st_file_is_older_than.md",
  "my_2nd_one_and_even_more_than.txt",
  "My_3rd_attempt.doc",
  NULL
};

UBYTE* dummyCol2[] =
{
  "File-1.md",
  "File-2.txt",
  "File-3.doc",
  NULL
};

struct List* createDummyFileList()
{
  struct Node *pNode;
  struct List* pFilesList;
  ULONG i = 0;
  UBYTE **ppLabels1 = dummyCol1;
  UBYTE **ppLabels2 = dummyCol2;

  if(NULL == (pFilesList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pFilesList);

  while (NULL != *ppLabels1)
  {
    if(*ppLabels2 == NULL)
    {
      break;
    }

    if (NULL != (pNode = AllocListBrowserNode(2, 
                                              LBNA_Column, 0,
                                                LBNCA_CopyText, FALSE,
                                                LBNCA_Editable, FALSE,
                                                LBNCA_MaxChars, 101,
                                                LBNCA_Text, (ULONG)*ppLabels1,
                                              LBNA_Column, 1,
                                                LBNCA_CopyText, FALSE,
                                                LBNCA_Editable, TRUE,
                                                LBNCA_MaxChars, 101,
                                                LBNCA_Text, (ULONG)*ppLabels2,
                                              TAG_DONE)))
    {
      AddTail(pFilesList, pNode);
    }
    else
    {
      break;
    }

    ppLabels1++;
    ppLabels2++;
    i++;
  }

  return pFilesList;
}


void freeFileList(struct List* pFilesList)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(NULL == pFilesList)
  {
    return;
  }

  pWorkNode = pFilesList->lh_Head;
  while(NULL != (pNextNode = pWorkNode->ln_Succ))
  {
    FreeListBrowserNode(pWorkNode);
    pWorkNode = pNextNode;
  }

  FreeVec(pFilesList);
}

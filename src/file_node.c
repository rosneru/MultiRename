#include <gadgets/chooser.h>
#include <gadgets/listbrowser.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/utility_protos.h>
  #include <clib/listbrowser_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
  #include <proto/utility.h>
  #include <proto/listbrowser.h>
#endif

#include "file_node.h"

struct List* createFileList(UBYTE** ppFileNames)
{
  struct Node *pNode;
  struct List* pFilesList;
  FileNode* pFileNode;
  ULONG i = 0;

  if(NULL == (pFilesList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pFilesList);

  while (NULL != *ppFileNames)
  {
    if (NULL != (pNode = AllocListBrowserNode(2, 
                                              LBNA_NodeSize, sizeof(FileNode),
                                              LBNA_Column, 0,
                                                LBNCA_CopyText, FALSE,
                                                LBNCA_Editable, FALSE,
                                                LBNCA_MaxChars, MAXNAMELEN,
                                              LBNA_Column, 1,
                                                LBNCA_CopyText, FALSE,
                                                LBNCA_Editable, FALSE,
                                                LBNCA_MaxChars, MAXNAMELEN,
                                              TAG_DONE)))
    {
      pFileNode = (FileNode*) pNode;
      Strncpy(pFileNode->OldName, *ppFileNames, MAXNAMELEN);
      Strncpy(pFileNode->NewName, *ppFileNames, MAXNAMELEN);
      SetListBrowserNodeAttrs(pNode,
                              LBNA_Column, 0,
                                LBNCA_Text, pFileNode->OldName,
                              LBNA_Column, 1,
                                LBNCA_Text, pFileNode->NewName,
                              TAG_DONE);
      AddTail(pFilesList, pNode);
    }
    else
    {
      break;
    }

    ppFileNames++;
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

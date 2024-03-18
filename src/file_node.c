#include <gadgets/chooser.h>
#include <gadgets/listbrowser.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/utility_protos.h>
  #include <clib/listbrowser_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/utility.h>
  #include <proto/listbrowser.h>
#endif

#include "file_node.h"



struct Node* createFileNode(STRPTR pFileName)
{
  STRPTR pPathEnd, pNameStart;
  ULONG pathLength;
  struct Node *pNode;
  FileNode* pFileNode;


  if ((pNode = AllocListBrowserNode(2, 
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

    pPathEnd = PathPart(pFileName);
    pNameStart = FilePart(pFileName);
    pathLength = pPathEnd - pFileName;
    if(pathLength > MAXPATHLEN)
    {
      // TODO: Notify truncation
      pathLength = MAXPATHLEN;
    }

    Strncpy(pFileNode->Path, pFileName, pathLength);
    Strncpy(pFileNode->OldName, pNameStart, MAXNAMELEN);
    Strncpy(pFileNode->NewName, pNameStart , MAXNAMELEN);
    SetListBrowserNodeAttrs(pNode,
                            LBNA_Column, 0,
                              LBNCA_Text, pFileNode->OldName,
                            LBNA_Column, 1,
                              LBNCA_Text, pFileNode->NewName,
                            TAG_DONE);
    return pNode;
  }
  else
  {
    return NULL;
  }
}

struct List* createFileList(STRPTR* ppFileNames)
{

  struct Node *pNode;
  struct List* pFilesList;
  FileNode* pFileNode;
  ULONG i = 0;

  if(!(pFilesList = AllocVec(sizeof(struct List), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pFilesList);

  if(!ppFileNames)
  {
    return pFilesList;
  }

  while (*ppFileNames)
  {
    if ((pNode = createFileNode(*ppFileNames)))
    {
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

  if(!pFilesList)
  {
    return;
  }

  pWorkNode = pFilesList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    FreeListBrowserNode(pWorkNode);
    pWorkNode = pNextNode;
  }

  FreeVec(pFilesList);
}

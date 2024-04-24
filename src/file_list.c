#include <gadgets/chooser.h>
#include <gadgets/listbrowser.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/listbrowser_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/listbrowser.h>
#endif

#include <string.h>
#include <stdio.h>

#include "notifications.h"
#include "file_list.h"


struct Node* createFileNode(STRPTR pFileName, struct List* pNotificationsList)
{
  STRPTR pPathEnd, pNameStart, pLastDotPosition;
  ULONG pathLen, nameLen;
  struct Node *pNode;
  FileNode* pFileNode;

  nameLen = strlen(pFileName);
  if(nameLen > 4)
  {
    if(pFileName[nameLen-1] == 'o'
    && pFileName[nameLen-2] == 'f'
    && pFileName[nameLen-3] == 'n'
    && pFileName[nameLen-4] == 'i'
    && pFileName[nameLen-5] == '.')
    {
      // It is an .info file. Skipping it..
      addNotification(pNotificationsList,
                      NNT_SKIPPED_INFO_FILE,
                      pFileName);
      return NULL;
    }
  }

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

    // Separate path (to pFileNode->Path) and file name (to pNameStart)
    pPathEnd = PathPart(pFileName);
    pNameStart = FilePart(pFileName);
    pathLen = pPathEnd - pFileName + 1;
    if(pathLen > MAXPATHLEN)
    {
      // TODO: Notify truncation
      pathLen = MAXPATHLEN;
    }

    strncpy(pFileNode->Path, pFileName, pathLen);
    pFileNode->Path[pathLen] = '\0';

    strcpy(pFileNode->OldName, pNameStart);
    strcpy(pFileNode->NewName, pNameStart);
    if((pLastDotPosition = strrchr(pNameStart, '.')))
    {
      pFileNode->OldNameLen = pLastDotPosition - pNameStart;
      pFileNode->OldExtLen = strlen(pFileNode->OldName + pFileNode->OldNameLen);
    }
    else
    {
      pFileNode->OldNameLen = strlen(pNameStart);
      pFileNode->OldExtLen = 0;
    }

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


struct List* createFileList(void)
{
  struct List* pFilesList;
  if(!(pFilesList = AllocVec(sizeof(struct List), MEMF_CLEAR)))
  {
    return NULL;
  }

  NewList(pFilesList);
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

FileNode* getLongestOldNameNode(struct List* pFilesList)
{
  struct Node* pNode;
  FileNode* pFileNode;
  FileNode* pMaxLengthNode = NULL;
  ULONG maxLength = 0;

  if(!pFilesList)
  {
    return NULL;
  }

  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    if(pFileNode->OldNameLen > maxLength)
    {
      maxLength = pFileNode->OldNameLen;
      pMaxLengthNode = pFileNode;
    }
  }

  return pMaxLengthNode;
}

void printFileListNewName(struct List* pFilesList)
{
  struct Node* pNode;
  FileNode* pFileNode;
  printf("New name list\n");
  printf("=============\n");
  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    printf("  %s\n", pFileNode->NewName);
  }

  printf("\n");
}

BOOL appendFileNode(struct List* pFilesList,
                    STRPTR pFileFullPath,
                    struct List* pNotificationsList)
{
  struct Node *pNode;
  STRPTR pFirstPath;

  if(!pFilesList || !pFileFullPath || !pNotificationsList)
  {
    return FALSE;
  }

  pFirstPath = getFirstFilePath(pFilesList);

  if(!(pNode = createFileNode(pFileFullPath, pNotificationsList)))
  {
    return FALSE;
  }

  if(pFirstPath && (strcmp(((FileNode*)pNode)->Path, pFirstPath) != 0))
  {
    // This file has a different path as the former ones: skip it
    addNotification(pNotificationsList,
                    NNT_SKIPPED_WRONG_PATH,
                    pFileFullPath);
    FreeListBrowserNode(pNode);
    return FALSE;
  }

  // This file has the same path as the former ones: add it
  AddTail(pFilesList, pNode);
  return TRUE;
}


STRPTR getFirstFilePath(struct List* pFilesList)
{
  if(NULL == pFilesList->lh_Head->ln_Succ)
  {
    return NULL;
  }

  return ((FileNode*)pFilesList->lh_Head)->Path;
}

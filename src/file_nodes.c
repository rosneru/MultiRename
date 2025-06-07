#include <gadgets/chooser.h>
#include <gadgets/listbrowser.h>
#include <libraries/locale.h>

#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/listbrowser_protos.h>
  #include <clib/locale_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/listbrowser.h>
  #include <proto/locale.h>
#endif

#include <string.h>
#include <stdio.h>

#include "notifications.h"
#include "file_tools.h"
#include "file_nodes.h"


/// D_S macro definition

/**
 * Thomas Richter @ RKRM DOS, 2024: It is certainly a burden to always
 * allocate temporary BCPL objects from the heap through the
 * exec.library or the os.library, and doing so can also fragment the
 * AmigaOs memory unnecessarily. However, allocation of automatic
 * objects from the stack does not ensure long-word alignment in
 * general. To work around this burden, one can use a trick and instead
 * request from the compiler a somewhat longer object with automatic
 * storage duration and align the requested object manually within the
 * memory obtained this way. The D_S macro (see below) performs this
 * trick.
 * 
 * It is used as follows:
 *     `D_S (struct FileInfoBlock, fib);`
 * 
 * At this point, fib is a pointer to a properly aligned struct
 * FileInfoBlock, e.g. this is equivalent to
 *     `struct FileInfoBlock _tmp;`
 *     `struct FileInfoBlock *fib = &tmp;`
 * Except that the created pointer is properly aligned and can safely be
 * passed into the dos.library.
 */
#define D_S(type,name) char a_##name[sizeof(type)+3]; \
                       type *name = (type *)((ULONG)(a_##name+3) & ~3UL)


///
/// Private function implementations

struct Node* createFileNode(struct Locale* pLocale,
                            BPTR pLock,
                            STRPTR pFileName,
                            struct List* pNotifications)
{
  STRPTR pPathEnd, pNameStart, pLastDotPosition;
  ULONG pathLen, nameLen;
  struct Node *pNode;
  FileNode* pFileNode;
  D_S(struct FileInfoBlock, pFib);  // See explanation of D_S macro above.

  nameLen = strlen(pFileName);
  if(nameLen > 4)
  {
    if(pFileName[nameLen-1] == 'o'
    && pFileName[nameLen-2] == 'f'
    && pFileName[nameLen-3] == 'n'
    && pFileName[nameLen-4] == 'i'
    && pFileName[nameLen-5] == '.')
    {
      return NULL;
    }
  }

  if(DOSFALSE == Examine(pLock, pFib))
  {
    addNotification(pNotifications,
                    NNT_SKIPPED_FAILED_EXAMINE,
                    pFileName);
    return NULL;
  }


  if ((pNode = AllocListBrowserNode(3, 
                                    LBNA_NodeSize, sizeof(FileNode),
                                    LBNA_Column, 0,
                                      LBNCA_CopyText, FALSE,
                                      LBNCA_Editable, FALSE,
                                      LBNCA_MaxChars, MAX_NAME_LEN,
                                    LBNA_Column, 1,
                                      LBNCA_CopyText, FALSE,
                                      LBNCA_Editable, FALSE,
                                      LBNCA_MaxChars, MAX_NAME_LEN,
                                    LBNA_Column, 2,
                                      LBNCA_CopyText, FALSE,
                                      LBNCA_Editable, FALSE,
                                      LBNCA_MaxChars, MAX_NAME_LEN,
                                    TAG_DONE)))
  {
    pFileNode = (FileNode*) pNode;

    // Separate path (to pFileNode->Path) and file name (to pNameStart)
    pPathEnd = PathPart(pFileName);
    pNameStart = FilePart(pFileName);
    pathLen = pPathEnd - pFileName + 1;
    if(pathLen > MAX_PATH_LEN)
    {
      // TODO: Notify truncation
      pathLen = MAX_PATH_LEN;
    }

    strncpy(pFileNode->Path, pFileName, pathLen);
    pFileNode->Path[pathLen] = '\0';

    strcpy(pFileNode->OriginalName, pNameStart);
    strcpy(pFileNode->NewName, pNameStart);
    if((pLastDotPosition = strrchr(pNameStart, '.')))
    {
      pFileNode->OriginalNameLen = pLastDotPosition - pNameStart;
      pFileNode->OriginalExtLen = strlen(pFileNode->OriginalName
                                          + pFileNode->OriginalNameLen
                                          + 1); // +1 for the dot '.'
    }
    else
    {
      pFileNode->OriginalNameLen = strlen(pNameStart);
      pFileNode->OriginalExtLen = 0;
    }


    if(!fillDateTimeParts(pLocale, &pFib->fib_Date, &pFileNode->OriginalDate))
    {
      FreeListBrowserNode(pNode);
      addNotification(pNotifications,
                      NNT_SKIPPED_FAILED_DATETIMEPARTS,
                      pFileName);
      return NULL;
    }

    SetListBrowserNodeAttrs(pNode,
                            LBNA_Column, 0,
                              LBNCA_Text, " ",
                            LBNA_Column, 1,
                              LBNCA_Text, pFileNode->OriginalName,
                            LBNA_Column, 2,
                              LBNCA_Text, pFileNode->NewName,
                            TAG_DONE);
    return pNode;
  }
  else
  {
    return NULL;
  }
}

///
/// Public function implementations

FileNodes* createFileNodes(void)
{
  FileNodes* pFiles;
  if(!(pFiles = AllocVec(sizeof(FileNodes), MEMF_CLEAR)))
  {
    return NULL;
  }

  if(!(pFiles->pList = AllocVec(sizeof(struct List), MEMF_CLEAR)))
  {
    freeFileNodes(pFiles);
    return NULL;
  }

  NewList(pFiles->pList);
  return pFiles;
}

void freeFileNodes(FileNodes* pFiles)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pFiles)
  {
    return;
  }

  if(pFiles->DirLock)
  {
    UnLock(pFiles->DirLock);
  }

  if(pFiles->pList)
  {
    pWorkNode = pFiles->pList->lh_Head;
    while((pNextNode = pWorkNode->ln_Succ))
    {
      FreeListBrowserNode(pWorkNode);
      pWorkNode = pNextNode;
    }

    FreeVec(pFiles->pList);
  }

  FreeVec(pFiles);
}

BPTR getFilesDirLock(FileNodes* pFiles)
{
  return pFiles->DirLock;
}

BOOL setFilesDirLock(FileNodes* pFiles, BPTR pFilesDirLock)
{
  BOOL result;
  if(pFiles->DirLock)
  {
    UnLock(pFiles->DirLock);
  }

  pFiles->DirLock = pFilesDirLock;
  result = NameFromLock(pFilesDirLock, pFiles->DirPath, MAX_PATH_LEN);
  return result;
}

char* getFilesDirPath(FileNodes* pFiles)
{
  return pFiles->DirPath;
}

ULONG countFileNodes(FileNodes* pFiles)
{
  ULONG count = 0;
  struct Node* pNode;

  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
      count++;
  }

  return count;
}

FileNode* getLongestOldNameNode(FileNodes* pFiles)
{
  struct Node* pNode;
  FileNode* pFileNode;
  FileNode* pMaxLengthNode = NULL;
  ULONG maxLength = 0;

  if(!pFiles->pList)
  {
    return NULL;
  }

  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    if(pFileNode->OriginalNameLen > maxLength)
    {
      maxLength = pFileNode->OriginalNameLen;
      pMaxLengthNode = pFileNode;
    }
  }

  return pMaxLengthNode;
}

FileNode* getLongestOldExtNode(FileNodes* pFiles)
{
  struct Node* pNode;
  FileNode* pFileNode;
  FileNode* pMaxLengthNode = NULL;
  ULONG maxLength = 0;

  if(!pFiles->pList)
  {
    return NULL;
  }

  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    if(pFileNode->OriginalExtLen > maxLength)
    {
      maxLength = pFileNode->OriginalExtLen;
      pMaxLengthNode = pFileNode;
    }
  }

  return pMaxLengthNode;
}

void printFileListOriginalName(FileNodes* pFiles)
{
  struct Node* pNode;
  FileNode* pFileNode;
  printf("** Original file list **\n");
  printf("Name                                   |date\n");
  printf("=======================================|=======================================\n");
  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    printf("%-39s|%s-%s-%s, %s:%s:%s\n", pFileNode->OriginalName,
                                         pFileNode->OriginalDate.pYear,
                                         pFileNode->OriginalDate.pMonth,
                                         pFileNode->OriginalDate.pDay,
                                         pFileNode->OriginalDate.pHour,
                                         pFileNode->OriginalDate.pMinute,
                                         pFileNode->OriginalDate.pSecond);
  }

  printf("\n\n");
}


void printFileListNewName(FileNodes* pFiles)
{
  struct Node* pNode;
  FileNode* pFileNode;
  printf("New name list\n");
  printf("=============\n");
  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    printf("  %s\n", pFileNode->NewName);
  }

  printf("\n");
}

BOOL appendFileNode(FileNodes* pFiles,
                    STRPTR pFileFullPath,
                    struct Locale* pLocale,
                    struct List* pNotifications)
{
  BPTR pLock;
  struct Node *pNode;
  STRPTR pWorkingPath;

  if(!pFiles || !pFiles->pList || !pFileFullPath || !pNotifications)
  {
    return FALSE;
  }

  if(!(pLock = lockFromLongName(pFileFullPath)))
  {
    addNotification(pNotifications,
                    NNT_SKIPPED_FAILED_LOCK,
                    pFileFullPath);
    return FALSE;
  }

  if(!(pNode = createFileNode(pLocale, pLock, pFileFullPath, pNotifications)))
  {
    return FALSE;
  }

  if((pWorkingPath = getFirstFilePath(pFiles))
  && (strcmp(((FileNode*)pNode)->Path, pWorkingPath) != 0))
  {
    // This file has a different path as the former ones: skip it
    addNotification(pNotifications,
                    NNT_SKIPPED_WRONG_PATH,
                    pFileFullPath);
    FreeListBrowserNode(pNode);
    return FALSE;
  }

  // File has the same path as the former ones: add it
  AddTail(pFiles->pList, pNode);
  UnLock(pLock);
  return TRUE;
}


STRPTR getFirstFilePath(FileNodes* pFiles)
{
  if(NULL == pFiles->pList->lh_Head->ln_Succ)
  {
    return NULL;
  }

  return ((FileNode*)pFiles->pList->lh_Head)->Path;
}

///

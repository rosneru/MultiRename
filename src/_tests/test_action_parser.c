#include <stdio.h>
#include <string.h>

#include <exec/lists.h>
#include <exec/types.h>
#include "../file_node.h"


void printFileList(struct List* pFilesList, const char* pTitle)
{
  struct Node* pNode;
  FileNode* pFileNode;
  printf("** %s **\n", pTitle);
  printf("Old name                               |New name\n");
  printf("=======================================|=======================================\n");
  for(pNode = pFilesList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    printf("%-39s|%-39s\n", pFileNode->OldName, pFileNode->NewName);
  }

  printf("\n\n");
}

FileNode node1 = {
  {0},
  "Shared:dev/projects/MultiRename/testdata/",
  "My_1st_file_is_older_than.md",
  "My_1st_file_is_older_than.md"
};

FileNode node2 =
{
  {0},
  "Shared:dev/projects/MultiRename/testdata/",
  "my_2nd_one_and_even_more_than.txt",
  "my_2nd_one_and_even_more_than.txt"
};

FileNode node3 =
{
  {0},
  "Shared:dev/projects/MultiRename/testdata/",
  "My_3rd_attempt.doc",
  "My_3rd_attempt.doc"
};


void AddTail(struct List *pList, struct Node *pNode)
{
    pNode->ln_Succ                = (struct Node *)&pList->lh_Tail;
    pNode->ln_Pred                = pList->lh_TailPred;
    pList->lh_TailPred->ln_Succ   = pNode;
    pList->lh_TailPred            = pNode;
}

int main(void)
{
  int i;
  struct List fileList;

  // Initialize the list (NEWLIST)
  fileList.lh_Head = (struct Node*)&fileList.lh_Tail;
  fileList.lh_Tail = NULL;
  fileList.lh_TailPred = (struct Node*)&fileList.lh_Head;

  AddTail(&fileList, (struct Node*) &node1);
  AddTail(&fileList, (struct Node*) &node2);
  AddTail(&fileList, (struct Node*) &node3);

  printFileList(&fileList, "Initial file list");
}

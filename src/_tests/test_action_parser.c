#include <stdio.h>
#include <string.h>

#include <exec/lists.h>
#include <exec/types.h>
#include "../file_node.h"
#include "../rename_algorithm.h"
#include "../rename_counter.h"

// In Linux test environment LONG has 64bit. So the 32bit Amiga-like
// environment is simulated here for the tests..
#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif

/**
 * Print given list of FileNodes
 */
void printFileList(struct List* pFilesList, const char* pTitle);

FileNode node1 = { {0}, "Shared:dev/projects/MultiRename/testdata/", "My_1st_file_is_older_than.md", 25, 2, "My_1st_file_is_older_than.md" };
FileNode node2 = { {0}, "Shared:dev/projects/MultiRename/testdata/", "my_2nd_one_and_even_more_than.txt", 29, 3, "my_2nd_one_and_even_more_than.txt" };
FileNode node3 = { {0}, "Shared:dev/projects/MultiRename/testdata/", "My_3rd_attempt.doc", 14, 3, "My_3rd_attempt.doc" };


int main(void)
{
  int i;
  struct List fileList;
  Counter counter;

  // Initialize the list
  NewList(&fileList);

  AddTail(&fileList, (struct Node*) &node1);
  AddTail(&fileList, (struct Node*) &node2);
  AddTail(&fileList, (struct Node*) &node3);

  initCounter(&counter, 2, 2, 3);
  printf("Testing a Counter(2, 2, 3) with init value and 2 iterations..\n");
  printf("  %s\n", getCounterValue(&counter));
  incrementCounter(&counter);
  printf("  %s\n", getCounterValue(&counter));
  incrementCounter(&counter);
  printf("  %s\n\n", getCounterValue(&counter));


  initCounter(&counter, LONG_MAX, 3, 10);
  printf("Testing a Counter(LONG_MAX, 3, 10) with init value and 4 iterations..\n");
  printf("  %s\n", getCounterValue(&counter));
  incrementCounter(&counter);
  printf("  %s\n", getCounterValue(&counter));
  incrementCounter(&counter);
  printf("  %s\n", getCounterValue(&counter));
  incrementCounter(&counter);
  printf("  %s\n", getCounterValue(&counter));
  incrementCounter(&counter);
  printf("  %s\n\n", getCounterValue(&counter));

  printf("sizeof(LONG) = %lu\n", sizeof(LONG));

  printFileList(&fileList, "Initial file list");

  // test_name_2
  if(createNewNames(&fileList, "Abc - [N]", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_name_2: 'Abc - [N]', '[E]'");
  }
  else
  {
    printf("Error in test test_name_2.\n");
  }

  // test_name_4
  if(createNewNames(&fileList, "Aa[N]Bb", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_name_4: 'Aa[N]Bb', '[E]'");
  }
  else
  {
    printf("Error in test test_name_4.\n");
  }

  // test_name_5
  if(createNewNames(&fileList, "", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_name_5 (Create empty names): '', '[E]'");
  }
  else
  {
    printf("Error in test test_name_5.\n");
  }

  // test_name_6
  if(createNewNames(&fileList, "Abc - [N", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_name_6: Error because unclosed name cmd: 'Abc - [N', '[E]'");
  }
  else
  {
    printf("Expected error in test test_name_6: Error because unclosed name cmd.\n");
  }

  // test_name_7
  if(createNewNames(&fileList, "Abc - [N]]", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_name_7: Error because double closed name cmd: 'Abc - [N', '[E]'");
  }
  else
  {
    printf("Expected error in test test_name_7: Error because double closed name cmd.\n");
  }

  // test_name_part_1
  if(createNewNames(&fileList, "[N4-6]", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_name_part_1: '[N4-6]', '[E]'");
  }
  else
  {
    printf("Error in test test_name_part_1.\n");
  }

  // test_counter_mixed_2
  if(createNewNames(&fileList, "[N4-6] New[C] [N8-29]", "[E]", 1, 1, 1))
  {
    printFileList(&fileList, "test_counter_mixed_2: '[N4-6] New[C] [N8-29]', '[E]'");
  }
  else
  {
    printf("Error in test test_counter_mixed_2.\n");
  }
}




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

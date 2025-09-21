#include <stdio.h>
#include <string.h>

#include <exec/lists.h>
#include <exec/types.h>
#include "../file_nodes.h"
#include "../rename_algorithm.h"
#include "../rename_counter.h"

// In Linux test environment LONG has 64bit. So the 32bit Amiga-like
// environment is simulated here for the tests..
#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif

#define MAX_NAME_LEN 107


/// Forwards

/**
 * Print all FileNodes of `pFilesList`, only the original names but with
 * dates
 */
void printFileListOriginalName(FileNodes* pFiles);

/**
 * Print all FileNodes of `pFilesList`, original and new names, but no
 * dates
 */
void printFileList(FileNodes* pFiles, const char* pTitle);

/**
 * Set the date time parts pointers in `pFileNode`
 * (`OriginalDate.pYear`, `OriginalDate.pMonth`, etc) to the appropriate
 * parts of `pDateString` which must have a length of 19 chars (one less
 * than DATETIMEBUF_SIZE, which is defined in date_tools.h) and be in
 * the format "2024-05-06-12-54-23"
 */
void fillFileNodesDateTime(FileNode* pFileNode, const char* pDateString);


/// Global variables
FileNode node1 = { {0}, "Shared:dev/projects/MultiRename/testdata/", "My_1st_file_is_older_than.md", 25, 2 };
FileNode node2 = { {0}, "Shared:dev/projects/MultiRename/testdata/", "my_2nd_one_and_even_more_than.txt", 29, 3 };
FileNode node3 = { {0}, "Shared:dev/projects/MultiRename/testdata/", "My_3rd_attempt.doc", 14, 3 };


int main(void)
{
  int i;
  FileNodes files;
  Counter counter;

  fillFileNodesDateTime(&node1, "1978-06-18-12-00-11");
  fillFileNodesDateTime(&node2, "1986-09-27-11-33-44");
  fillFileNodesDateTime(&node3, "2017-11-23-22-38-22");

  // Initialize the list
  NewList(files.pList);

  AddTail(files.pList, (struct Node*) &node1);
  AddTail(files.pList, (struct Node*) &node2);
  AddTail(files.pList, (struct Node*) &node3);

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

  printFileListOriginalName(&files);

  // test_name_2
  if(createNewNames(&files, MAX_NAME_LEN, "Abc - [N]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_name_2: 'Abc - [N]', '[E]'");
  }
  else
  {
    printf("Error in test_name_2.\n");
  }

  // test_name_4
  if(createNewNames(&files, MAX_NAME_LEN, "Aa[N]Bb", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_name_4: 'Aa[N]Bb', '[E]'");
  }
  else
  {
    printf("Error in test_name_4.\n");
  }

  // test_name_5
  if(createNewNames(&files, MAX_NAME_LEN, "", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_name_5 (Create empty names): '', '[E]'");
  }
  else
  {
    printf("Error in test_name_5.\n");
  }

  // test_name_6
  if(createNewNames(&files, MAX_NAME_LEN, "Abc - [N", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_name_6: Error because unclosed name cmd: 'Abc - [N', '[E]'");
  }
  else
  {
    printf("Expected error in test_name_6: Error because unclosed name cmd.\n");
  }

  // test_name_7
  if(createNewNames(&files, MAX_NAME_LEN, "Abc - [N]]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_name_7: Error because double closed name cmd: 'Abc - [N', '[E]'");
  }
  else
  {
    printf("Expected error in test_name_7: Error because double closed name cmd.\n");
  }

  // test_name_part_1
  if(createNewNames(&files, MAX_NAME_LEN, "[N4-6]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_name_part_1: '[N4-6]', '[E]'");
  }
  else
  {
    printf("Error in test_name_part_1.\n");
  }

  // test_counter_mixed_2
  if(createNewNames(&files, MAX_NAME_LEN, "[N4-6] New[C] [N8-29]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_counter_mixed_2: '[N4-6] New[C] [N8-29]', '[E]'");
  }
  else
  {
    printf("Error in test_counter_mixed_2.\n");
  }

  // test_SegmentationFault_resulting_name_too_long
  if(createNewNames(&files, MAX_NAME_LEN, "[N][N][N][N][N]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_SegmentationFault_resulting_name_too_long: '[N][N][N][N][N]', '[E]'");
  }
  else
  {
    printf("Error in test_SegmentationFault_resulting_name_too_long.\n");
  }

  // test_date_1
  if(createNewNames(&files, MAX_NAME_LEN, "[YMD]-[hms]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_date_1: '[YMD]-[hms]', '[E]'");
  }
  else
  {
    printf("Error in test_date_1.\n");
  }

  // test_date_2
  if(createNewNames(&files, MAX_NAME_LEN, "[Y]-[M]-[D] - [hms]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_date_2: '[Y]-[M]-[D] - [hms]', '[E]'");
  }
  else
  {
    printf("Error in test_date_2.\n");
  }

  // test_date_3
  if(createNewNames(&files, MAX_NAME_LEN, "[YYY]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_date_3: '[YYY]', '[E]'");
  }
  else
  {
    printf("Error in test_date_3.\n");
  }

  // test_date_4
  if(createNewNames(&files, MAX_NAME_LEN, "[Y][Y][Y]", "[E]", 1, 1, 1))
  {
    printFileList(&files, "test_date_4: '[Y][Y][Y]', '[E]'");
  }
  else
  {
    printf("Error in test_date_4.\n");
  }
}




void printFileList(FileNodes* pFiles, const char* pTitle)
{
  struct Node* pNode;
  FileNode* pFileNode;
  char truncationState;

  printf("** %s **\n", pTitle);
  printf("Trunc|Original name                          |New name\n");
  printf("=====|=======================================|=======================================\n");
  for(pNode = pFiles->pList->lh_Head; pNode->ln_Succ; pNode = pNode->ln_Succ)
  {
    pFileNode = (FileNode*)pNode;
    if(pFileNode->IsNewNameTruncated)
    {
      truncationState = '*';
    }
    else
    {
      truncationState = ' ';
    }

    printf("  %c  |%-39s|%-39s\n", truncationState,
                                   pFileNode->OriginalName,
                                   pFileNode->NewName);
  }

  printf("\n\n");
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


void fillFileNodesDateTime(FileNode* pFileNode, const char* pDateString)
{
  char* pBuf;
  if(!pFileNode || !pDateString)
  {
    return;
  }

  if(strlen(pDateString) != (DATETIMEBUF_SIZE - 1))
  {
    return;
  }

  pBuf = pFileNode->OriginalDate.dateBuf;
  strcpy(pBuf, pDateString);

  // Manually overwrite the separator ('-' or ':') after each part
  // with \0. So every part has its own string finalizer.
  pBuf[4]  = '\0';
  pBuf[7]  = '\0';
  pBuf[10] = '\0';
  pBuf[13] = '\0';
  pBuf[16] = '\0';

  // Manually adjust the start pointers of the DateTimeParts to its
  // positions in pDateTimeBuf.
  pFileNode->OriginalDate.pYear   = pBuf;
  pFileNode->OriginalDate.pMonth  = pBuf + 5;
  pFileNode->OriginalDate.pDay    = pBuf + 8;
  pFileNode->OriginalDate.pHour   = pBuf + 11;
  pFileNode->OriginalDate.pMinute = pBuf + 14;
  pFileNode->OriginalDate.pSecond = pBuf + 17;
}

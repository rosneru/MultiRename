#include <stdio.h>

#include "amiga-types.h"
#include "../file_node.h"

int main(void)
{
  int i;

  printf("That's what I want:-)\n");
  for(i=0; i < 10; i++)
  {
    printf("  %d\n", i);
  }
}

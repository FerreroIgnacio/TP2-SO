#include "syscall.h"
#include "test_util.h"
// #include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include "../standard/standard.h"
#include "../process/process.h"
#include "../memory/memory.h"
#include "../fileDescriptorUtils/fileDescriptorUtils.h"

#define MAX_BLOCKS 128

typedef struct MM_rq
{
  void *address;
  uint32_t size;
} mm_rq;

int test_mm(int max_memory)
{
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

  if (max_memory <= 0)
    return -1;

  while (1)
  {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory)
    {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address)
      {
        printf("test_mm: SUCCESS Allocating %d bytes for block %d\n", mm_rqs[rq].size, rq);
        total += mm_rqs[rq].size;
        rq++;
      }
      else
      {
        printf("test_mm: ERROR Allocating %d bytes for block %d\n", mm_rqs[rq].size, rq);
      }
    }
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
        {
          printf("test_mm: ERROR\n");
          return -1;
        }
        else
        {
          printf("test_mm: SUCCESS writing and reading on block %d\n", rq);
        }
    // Free
    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
      {
        free(mm_rqs[i].address);
        printf("test_mm: SUCCESS freeing %d bytes\n", mm_rqs[i].size);
      }
    }
  }
  return -1;
}
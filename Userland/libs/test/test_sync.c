#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include <stdlib.h>
#include "../mystring/mystring.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc)
{
  uint64_t aux = *p;
  my_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[])
{
  uint64_t n;
  int64_t inc;
  int8_t use_sem;
  int64_t aux;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  printf("my_process_inc called with n = %d , inc = %d , use_sem = %d\n", n, inc, use_sem);

  if (use_sem)
    if ((aux = my_sem_open(SEM_ID, 1)) < 0)
    {
      printf("test_sync: ERROR opening semaphore %d\n", aux);
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem) {
      aux = my_sem_wait(SEM_ID);
      printf("return of wait: %d   iteration: %d\n", aux, i);
    }
    slowInc(&global, inc);
    if (use_sem) {
      aux = my_sem_post(SEM_ID);
      printf("return of post: %d   iteration: %d\n", aux, i);
    }
  }

  if (use_sem)
    my_sem_close(SEM_ID);

  return 0;
}

int test_sync(int iter, int use_sem)
{
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  // Duplicar argumentos para que los hijos no lean memoria de stack modificada
  char *n_copy = itoa_malloc(iter);          // strlen(argv[0]) + 1);
  char *use_sem_copy = itoa_malloc(use_sem); // strlen(argv[1]) + 1);
  if (n_copy == NULL || use_sem_copy == NULL)
  {
    free(n_copy);
    free(use_sem_copy);
    return -1;
  }

  char *argvDec[] = {n_copy, "-1", use_sem_copy, NULL};
  char *argvInc[] = {n_copy, "1", use_sem_copy, NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pids[i] = my_create_process("my_process_inc", 3, argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", 3, argvInc);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    my_wait(pids[i]);
    my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  printf("Final value: %d\n", global);

  free(n_copy);
  free(use_sem_copy);

  return 0;
}
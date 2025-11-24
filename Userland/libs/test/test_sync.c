#include <stdint.h>
#include "syscall.h"
#include "test_util.h"
#include <stdlib.h>
#include "../fileDescriptorUtils/fileDescriptorUtils.h"
#include "../mystring/mystring.h"
#include "../process/process.h"

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
  int8_t inc;
  int8_t use_sem;
  int64_t ret_wait, ret_post;
  int64_t proc_id;
  int64_t sem_open;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if ((sem_open = my_sem_open(SEM_ID, 1)) < 0)
    {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  proc_id = my_getpid();
  printf("pid: %d | Open: %d\n", proc_id, sem_open);
  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem)
    {
      ret_wait = my_sem_wait(SEM_ID);
    }
    slowInc(&global, inc);
    printf("pid: %d | iter: %d | global: %d | inc: %d | wait: %d | ", proc_id, i, global, inc, ret_wait);
    if (use_sem)
    {
      ret_post = my_sem_post(SEM_ID);
      printf("post: %d\n", ret_post);
    }
  }

  if (use_sem)
    printf("pid: %d | Close: %d\n", proc_id, my_sem_close(SEM_ID));

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[])
{
  pid_t pids[2 * TOTAL_PAIR_PROCESSES];
  int pipes[2 * TOTAL_PAIR_PROCESSES];
  int proc_running = 0;

  if (argc != 2)
    return -1;

  // Duplicar argumentos para que los hijos no lean memoria de stack modificada
  char *n_copy = malloc(strlen(argv[0]) + 1);
  char *use_sem_copy = malloc(strlen(argv[1]) + 1);
  if (n_copy == NULL || use_sem_copy == NULL)
  {
    free(n_copy);
    free(use_sem_copy);
    return -1;
  }
  strcpy(n_copy, argv[0]);
  strcpy(use_sem_copy, argv[1]);

  char *argvDec[] = {n_copy, "-1", use_sem_copy, NULL};
  char *argvInc[] = {n_copy, "1", use_sem_copy, NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pipes[i] = pipe_create();
    pids[i] = my_create_process("my_process_inc", 3, argvDec);
    pipes[i + TOTAL_PAIR_PROCESSES] = pipe_create();
    pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", 3, argvInc);

    if (pipes[i] < 0 || pipes[i + TOTAL_PAIR_PROCESSES] < 0 || pids[i] <= 1 || pids[i + TOTAL_PAIR_PROCESSES] <= 1)
    {
      printf("ERROR CREANDO PROCESOS\n");
      exit(-1);
    }

    fd_bind_std(pids[i], STDOUT, pipes[i]);
    fd_bind_std(pids[i + TOTAL_PAIR_PROCESSES], STDOUT, pipes[i + TOTAL_PAIR_PROCESSES]);

    proc_running += 2;
  }

  while (proc_running)
  {
    for (i = 0; i < TOTAL_PAIR_PROCESSES * 2; i++)
    {
      if (fd_has_data(i))
      {
        unsigned char buffer[STD_BUFF_SIZE];
        read(pipes[i], buffer, STD_BUFF_SIZE);
        printf("%s", buffer);
      }
    }
    pid_t terminated = my_wait(0);
    if (terminated <= 1)
    {
      continue;
    }
    else
    {
      proc_running--;
    }
    yield();
  }
  printf("Final value: %d\n", global);

  free(n_copy);
  free(use_sem_copy);

  return 0;
}
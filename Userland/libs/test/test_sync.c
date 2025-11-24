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
  int64_t ret_wait = 0, ret_post = 0;
  int64_t proc_id;
  int64_t sem_open = -1;

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
    else
    {
      printf("\n");
    }
  }

  if (use_sem)
    printf("pid: %d | Close: %d\n", proc_id, my_sem_close(SEM_ID));

  return 0;
}

int test_sync(int iter, int use_sem)
{
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
  int pipes[2 * TOTAL_PAIR_PROCESSES];
  int proc_running = 0;

  // Duplicar argumentos para que los hijos no lean memoria de stack modificada
  char *n_copy = itoa_malloc(iter);
  char *use_sem_copy = itoa_malloc(use_sem);
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

  // crear pipes
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pipes[i] = pipe_create();
    pipes[i + TOTAL_PAIR_PROCESSES] = pipe_create();

    if (pipes[i] < 0 || pipes[i + TOTAL_PAIR_PROCESSES] < 0)
    {
      printf("ERROR CREANDO PIPES\n");
      free(n_copy);
      free(use_sem_copy);
      return -1;
    }
  }

  // crear procesos y enlazar su stdout a un pipe
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pids[i] = my_create_process("my_process_inc", 3, argvDec);
    if (pids[i] <= 1)
    {
      printf("ERROR CREANDO PROCESO DEC %d\n", i);
      free(n_copy);
      free(use_sem_copy);
      return -1;
    }
    if (fd_bind_std(pids[i], STDOUT, pipes[i]) < 0)
    {
      printf("ERROR BINDING STDOUT proceso %d\n", pids[i]);
    }
    proc_running++;

    pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", 3, argvInc);
    if (pids[i + TOTAL_PAIR_PROCESSES] <= 1)
    {
      printf("ERROR CREANDO PROCESO INC %d\n", i);
      free(n_copy);
      free(use_sem_copy);
      return -1;
    }

    if (fd_bind_std(pids[i + TOTAL_PAIR_PROCESSES], STDOUT, pipes[i + TOTAL_PAIR_PROCESSES]) < 0)
    {
      printf("ERROR BINDING STDOUT proceso %d\n", pids[i + TOTAL_PAIR_PROCESSES]);
    }
    proc_running++;
  }

  // loop de lectura de los pipes
  while (proc_running > 1)
  {
    // Leer de todos los pipes que tengan datos
    for (i = 0; i < TOTAL_PAIR_PROCESSES * 2; i++)
    {
      if (pipe_available(pipes[i]) > 0)
      {
        unsigned char buffer[STD_BUFF_SIZE];
        // Leer del PIPE, no del índice i
        int bytes_read = pipe_read(pipes[i], (char *)buffer, STD_BUFF_SIZE - 1);
        if (bytes_read > 0)
        {
          buffer[bytes_read] = '\0'; // Null-terminate
          printf("%s", buffer);
        }
      }
    }

    // Verificar si algún proceso terminó
    pid_t terminated = my_wait(0);
    if (terminated > 0)
    {
      proc_running--;
    }

    // Dar chance a otros procesos
    yield();
  }

  printf("Final value: %d\n", global);

  free(n_copy);
  free(use_sem_copy);

  return 0;
}
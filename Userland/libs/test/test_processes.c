#include <stdio.h>
#include "syscall.h"
#include "test_util.h"

enum State
{
  RUNNING,
  BLOCKED,
  KILLED
};

typedef struct P_rq
{
  int32_t pid;
  enum State state;
} p_rq;

int test_processes(int max_processes)
{
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  char *argvAux[] = {0};

  if (max_processes <= 0)
    return -1;

  p_rq p_rqs[max_processes];

  while (1)
  {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++)
    {
      p_rqs[rq].pid = my_create_process("endless_loop", 0, argvAux);

      if (p_rqs[rq].pid == -1)
      {
        printf("test_processes: ERROR creating process %d\n", rq + 1);
        return -1;
      }
      else
      {
        p_rqs[rq].state = RUNNING;
        alive++;
        printf("test_processes: SUCCESS creating process %d\n", p_rqs[rq].pid);
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0)
    {

      for (rq = 0; rq < max_processes; rq++)
      {
        action = GetUniform(100) % 2;

        switch (action)
        {
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
          {
            int kill_value = my_kill(p_rqs[rq].pid);
            if (kill_value == -1)
            {
              printf("test_processes: ERROR killing process %d\n", rq + 1);
              return -1;
            }
            else if (kill_value == 0)
            {
              printf("test_processes: SUCCESS killing process %d\n", p_rqs[rq].pid);
            }
            int wait_value = my_wait(p_rqs[rq].pid);
            if (wait_value == -1)
            {
              printf("test_processes: ERROR waiting process %d\n", rq + 1);
              return -1;
            }
            else
            {
              printf("test_processes: SUCCESS waiting process %d ended with state %d\n", p_rqs[rq].pid, wait_value);
            }
            p_rqs[rq].state = KILLED;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING)
          {
            int block_value = my_block(p_rqs[rq].pid);
            if (block_value == -1)
            {
              printf("test_processes: ERROR blocking process %d\n", rq + 1);
              return -1;
            }
            else if (block_value == 0)
            {
              printf("test_processes: SUCCESS blocking process %d\n", p_rqs[rq].pid);
            }
            p_rqs[rq].state = BLOCKED;
          }
          break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
        {
          if (my_unblock(p_rqs[rq].pid) == -1)
          {
            printf("test_processes: ERROR unblocking process %d\n", rq + 1);
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}
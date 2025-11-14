#include <stdint.h>
#include <stddef.h>

#include "../process/process.h"
#include "../memory/memory.h"
#include "../semaphores/semaphores.h"
#include "../mystring/mystring.h"
#include "test_util.h"

typedef uint64_t (*test_entry_fn_t)(uint64_t argc, char *argv[]);

typedef struct
{
  test_entry_fn_t fn;
  uint64_t argc;
  char **argv;
} test_process_payload_t;

static int test_process_trampoline(void *arg)
{
  test_process_payload_t *payload = (test_process_payload_t *)arg;
  if (payload == NULL || payload->fn == NULL)
  {
    return -1;
  }

  uint64_t ret = payload->fn(payload->argc, payload->argv);
  free(payload);
  return (int)ret;
}

extern void zero_to_max(void);
extern uint64_t my_process_inc(uint64_t argc, char *argv[]);

static uint64_t endless_loop_wrapper(uint64_t argc, char *argv[])
{
  (void)argc;
  (void)argv;
  endless_loop();
  return 0;
}

static uint64_t zero_to_max_wrapper(uint64_t argc, char *argv[])
{
  (void)argc;
  (void)argv;
  zero_to_max();
  return 0;
}

static uint64_t my_process_inc_wrapper(uint64_t argc, char *argv[])
{
  return my_process_inc(argc, argv);
}

typedef struct
{
  const char *name;
  test_entry_fn_t fn;
} test_process_descriptor_t;

static const test_process_descriptor_t test_process_table[] = {
    {"endless_loop", endless_loop_wrapper},
    {"zero_to_max", zero_to_max_wrapper},
    {"my_process_inc", my_process_inc_wrapper},
};

static test_entry_fn_t lookup_test_entry(const char *name)
{
  if (name == NULL)
  {
    return NULL;
  }

  for (unsigned int i = 0; i < sizeof(test_process_table) / sizeof(test_process_table[0]); i++)
  {
    if (strcmp(name, test_process_table[i].name) == 0)
    {
      return test_process_table[i].fn;
    }
  }

  return NULL;
}

#define MAX_TEST_SEMAPHORES 8

typedef struct
{
  char *name;
  int sem_handle;
} test_named_sem_t;

static test_named_sem_t sem_table[MAX_TEST_SEMAPHORES];

static char *dup_string(const char *src)
{
  if (src == NULL)
  {
    return NULL;
  }
  size_t len = strlen(src) + 1;
  char *copy = (char *)malloc(len);
  if (copy == NULL)
  {
    return NULL;
  }
  memcpy(copy, src, len);
  return copy;
}

static int find_sem_index(const char *name)
{
  if (name == NULL)
  {
    return -1;
  }
  for (int i = 0; i < MAX_TEST_SEMAPHORES; i++)
  {
    if (sem_table[i].name != NULL && strcmp(sem_table[i].name, name) == 0)
    {
      return i;
    }
  }
  return -1;
}

static int store_sem_handle(const char *name, int handle)
{
  for (int i = 0; i < MAX_TEST_SEMAPHORES; i++)
  {
    if (sem_table[i].name == NULL)
    {
      sem_table[i].name = dup_string(name);
      if (sem_table[i].name == NULL)
      {
        return -1;
      }
      sem_table[i].sem_handle = handle;
      return 0;
    }
  }
  return -1;
}

static int get_sem_handle(const char *name)
{
  int idx = find_sem_index(name);
  if (idx < 0)
  {
    return -1;
  }
  return sem_table[idx].sem_handle;
}

static void forget_sem_handle(const char *name)
{
  int idx = find_sem_index(name);
  if (idx < 0)
  {
    return;
  }
  free(sem_table[idx].name);
  sem_table[idx].name = NULL;
  sem_table[idx].sem_handle = -1;
}

int64_t my_getpid()
{
  return getpid();
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[])
{
  test_entry_fn_t entry = lookup_test_entry(name);
  if (entry == NULL)
  {
    return -1;
  }

  test_process_payload_t *payload = (test_process_payload_t *)malloc(sizeof(test_process_payload_t));
  if (payload == NULL)
  {
    return -1;
  }

  payload->fn = entry;
  payload->argc = argc;
  payload->argv = argv;

  // TODO: CORREGIR PROCESOS
  int pid = new_proc(test_process_trampoline, payload);

  if (pid < 0)
  {
    free(payload);
    return -1;
  }
  return pid;
}

int64_t my_nice(uint64_t pid, uint64_t newPrio)
{
  return set_priority((pid_t)pid, (process_priority_t)newPrio);
}

int64_t my_kill(uint64_t pid)
{
  return kill((pid_t)pid);
}

int64_t my_block(uint64_t pid)
{
  return block_proc((pid_t)pid);
}

int64_t my_unblock(uint64_t pid)
{
  return unblock_proc((pid_t)pid);
}

int64_t my_sem_open(char *sem_id, uint64_t initialValue)
{
  if (sem_id == NULL)
  {
    return -1;
  }

  int existing_idx = find_sem_index(sem_id);
  if (existing_idx >= 0)
  {
    return sem_table[existing_idx].sem_handle;
  }

  int handle = sem_open(sem_id, (int)initialValue);
  if (handle < 0)
  {
    return -1;
  }

  if (store_sem_handle(sem_id, handle) < 0)
  {
    sem_close(handle);
    return -1;
  }

  return handle;
}

int64_t my_sem_wait(char *sem_id)
{
  int handle = get_sem_handle(sem_id);
  if (handle < 0)
  {
    return -1;
  }
  return sem_wait(handle);
}

int64_t my_sem_post(char *sem_id)
{
  int handle = get_sem_handle(sem_id);
  if (handle < 0)
  {
    return -1;
  }
  return sem_post(handle);
}

int64_t my_sem_close(char *sem_id)
{
  int handle = get_sem_handle(sem_id);
  if (handle < 0)
  {
    return -1;
  }

  int result = sem_close(handle);
  if (result == 0)
  {
    forget_sem_handle(sem_id);
  }
  return result;
}

int64_t my_yield()
{
  yield();
  return 0;
}

int64_t my_wait(int64_t pid)
{
  int status = 0;
  return waitpid((pid_t)pid, &status, WHANG);
}

#ifndef KERNEL_INCLUDE_SEM_H
#define KERNEL_INCLUDE_SEM_H

#include <stdint.h>

int sem_open(const char *name, int initial_value);
int sem_close(int sem_id);
int sem_wait(int sem_id);
int sem_post(int sem_id);
void sem_set(int sem_id, int value);

// Used by scheduler/exit paths to remove dangling waiters.
void sem_cleanup_dead_process(int pid);

#endif // KERNEL_INCLUDE_SEM_H

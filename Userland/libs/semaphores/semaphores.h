#ifndef SEMAPHORES_H
#define SEMAPHORES_H

int sem_open(const char *name, int initial_value);
int sem_wait(int sem_id);
int sem_post(int sem_id);
int sem_close(int sem_id);
void sem_set(int sem_id, int new_value);

#endif
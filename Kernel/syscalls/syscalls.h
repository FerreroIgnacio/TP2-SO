#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>
#include "../scheduler/scheduler.h"
#include "../videoDriver/videoDriver.h"
#include "../filesDescriptors/fd.h" // for fd_info_t

#define STDIN 0
#define STDOUT 1
#define STDERR 2 // standard per-process FDs; dynamic also start at 0 using fd subsystem

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_ISKEYPRESSED 2
#define SYSCALL_GET_BOOTTIME 3
#define SYSCALL_GET_TIME 4
#define SYSCALL_GET_DATE 5
#define SYSCALL_GET_REGISTERS 6

#define SYSCALL_GET_FB 7
#define SYSCALL_SET_FB 8
#define SYSCALL_FREE_FB 9

#define SYSCALL_PLAY_SOUND 20
#define SYSCALL_STOP_SOUND 21

#define SYSCALL_MALLOC 30
#define SYSCALL_CALLOC 31
#define SYSCALL_REALLOC 32
#define SYSCALL_FREE 33
#define SYSCALL_GET_MEMORY_INFO 34

// Spawnear/Iniciar una nueva tarea (process init)
#define SYSCALL_PROC_CREATE 40
// Terminar proceso
#define SYSCALL_PROC_EXIT 41
// Obtener process ID
#define SYSCALL_GET_PID 42
// Listar tareas: arg1=proc_info_t* out, arg2=int max; retorna cantidad
#define SYSCALL_PROC_LIST 43
// Matar/Killear una tarea por pid
#define SYSCALL_PROC_KILL 44
// Modificar prioridad de un proceso
#define SYSCALL_SET_PRIORITY 45
// Obtener la prioridad de un proceso
#define SYSCALL_GET_PRIORITY 46
// Bloquear proceso
#define SYSCALL_BLOCK_PROCESS 47
// Desbloquear un proceso
#define SYSCALL_UNBLOCK_PROCESS 48
// Renunciar al cpu
#define SYSCALL_YIELD 49
// Esperar a que los hijos terminen
#define SYSCALL_WAIT_PID 50
// Dormir ms milisegundos
#define SYSCALL_SLEEP 51

// Semáforos
#define SYSCALL_SEM_OPEN 60
#define SYSCALL_SEM_WAIT 61
#define SYSCALL_SEM_POST 62
#define SYSCALL_SEM_CLOSE 63
#define SYSCALL_SEM_SET 64

// File Descriptors dynamic
#define SYSCALL_FD_OPEN 70
#define SYSCALL_FD_LIST 71
#define SYSCALL_PIPE_CREATE 72
#define SYSCALL_FD_BIND_STD 73

int sys_read(int fd, char *buffer, uint64_t count);

int sys_write(int fd, const char *buffer, uint64_t count);

int sys_isKeyPressed(uint16_t makecode);

uint64_t sys_getBootTime();

void sys_getTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

void sys_getDate(uint8_t *year, uint8_t *month, uint8_t *day);

void saveRegisters(uint64_t *regs);

void sys_getRegisters(uint64_t *regs);

frameBuffer_adt sys_get_fb();

void sys_set_fb(uint8_t index);

void sys_free_fb(uint8_t index);

void sys_playSound(uint16_t freq);
void sys_stopSound();

void *sys_malloc(uint64_t size);
void *sys_calloc(uint64_t count, uint64_t size);
void *sys_realloc(void *ptr, uint64_t size);
void sys_free(void *ptr);
void sys_get_memory_info(uint64_t *total, uint64_t *used, uint64_t *free);

int sys_proc_create(task_fn_t entry, void *argv);
void sys_proc_exit(int exit_code);
int sys_get_pid(void);
int sys_proc_list(proc_info_t *out, int max);
int sys_proc_kill(int pid);
int sys_set_priority(int pid, int new_priority);
int sys_get_priority(int pid);
int sys_block_proc(int pid);
int sys_unblock_proc(int pid);
void sys_yield(void);
int sys_waitpid(int pid, int *status, int options);
void sys_sleep(int ms);

int sys_sem_open(const char *name, int initial_value);
int sys_sem_wait(int sem_id);
int sys_sem_post(int sem_id);
int sys_sem_close(int sem_id);
void sys_sem_set(int sem_id, int new_value);

int sys_fd_open(const char *name);
int sys_fd_list(fd_info_t *out, int max);
int sys_pipe_create(void);
int sys_fd_bind_std(int pid, int which, int pipe_id);

#endif

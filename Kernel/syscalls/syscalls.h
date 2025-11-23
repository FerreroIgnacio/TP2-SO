#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>
#include "../scheduler/scheduler.h"
#include "../videoDriver/videoDriver.h"
#include "../filesDescriptors/fd.h"
#include "../filesDescriptors/pipes.h"

// Descriptores estandar por proceso
#define STDIN 0
#define STDOUT 1
#define STDERR 2 // standard per-process FDs; dynamic also start at 0 using fd subsystem

// Tabla de numeros de syscall
#define SYSCALL_READ 0          // (fd:int, buf:char*, count:u64) -> int bytes leidos (parcial) o -1
#define SYSCALL_WRITE 1         // (fd:int, buf:char*, count:u64) -> int bytes escritos (parcial) o -1
#define SYSCALL_ISKEYPRESSED 2  // (makecode:u16) -> int 0/1
#define SYSCALL_GET_BOOTTIME 3  // () -> ticks*55ms
#define SYSCALL_GET_TIME 4      // (h:*u8, m:*u8, s:*u8)
#define SYSCALL_GET_DATE 5      // (y:*u8, m:*u8, d:*u8)
#define SYSCALL_GET_REGISTERS 6 // (regs:*u64[15])

#define SYSCALL_GET_FB 7        // () -> frameBuffer_adt
#define SYSCALL_SET_FB 8        // (index:u8)
#define SYSCALL_FREE_FB 9       // (index:u8)

#define SYSCALL_PLAY_SOUND 20   // (freq:u16)
#define SYSCALL_STOP_SOUND 21   // ()

#define SYSCALL_MALLOC 30       // (size:u64) -> void*
#define SYSCALL_CALLOC 31       // (count:u64, size:u64) -> void*
#define SYSCALL_REALLOC 32      // (ptr:void*, size:u64) -> void*
#define SYSCALL_FREE 33         // (ptr:void*)
#define SYSCALL_GET_MEMORY_INFO 34 // (total:*size_t, used:*size_t, free:*size_t)

// Procesos
#define SYSCALL_PROC_CREATE 40  // (entry:task_fn_t, argv:void*) -> pid o -1
#define SYSCALL_PROC_EXIT 41    // (exit_code:int)
#define SYSCALL_GET_PID 42      // () -> pid
#define SYSCALL_PROC_LIST 43    // (out:proc_info_t*, max:int) -> cant copiada
#define SYSCALL_PROC_KILL 44    // (pid:int) -> 0 ok, -1 err
#define SYSCALL_SET_PRIORITY 45 // (pid:int, prio:int) -> 0 ok, -1 err
#define SYSCALL_GET_PRIORITY 46 // (pid:int) -> prio o -1
#define SYSCALL_BLOCK_PROCESS 47// (pid:int) -> 0 ok, -1 err
#define SYSCALL_UNBLOCK_PROCESS 48 // (pid:int) -> 0 ok, -1 err
#define SYSCALL_YIELD 49        // ()
#define SYSCALL_WAIT_PID 50     // (pid:int, status:*int, options:int) -> pid o -1
#define SYSCALL_SLEEP 51        // (ms:int)

// Semáforos
#define SYSCALL_SEM_OPEN 60     // (name:const char*, initial:int) -> sem_id o -1
#define SYSCALL_SEM_WAIT 61     // (sem_id:int) -> 0 ok, -1 err (bloquea si valor<=0)
#define SYSCALL_SEM_POST 62     // (sem_id:int) -> 0 ok, -1 err
#define SYSCALL_SEM_CLOSE 63    // (sem_id:int) -> 0 ok, -1 err
#define SYSCALL_SEM_SET 64      // (sem_id:int, new_value:int)

// File Descriptors dinámicos y pipes
#define SYSCALL_FD_OPEN 70      // (name:const char*) -> fd dinamico o -1
#define SYSCALL_FD_LIST 71      // (out:fd_info_t*, max:int) -> cant (del proceso actual)
#define SYSCALL_FD_LIST_PID 78  // (pid:int, out:fd_info_t*, max:int) -> cant (FDs de pid)
#define SYSCALL_PIPE_CREATE 72  // () -> pipe_id o -1
#define SYSCALL_FD_BIND_STD 73  // (pid:int, which:int{0:stdin,1:stdout}, pipe_id:int) -> 0/-1
#define SYSCALL_PIPE_WRITE 74   // (pipe_id:int, buf:const char*, count:u64) -> bytes escritos o -1 (parcial permitido). En pipe 0 retorna -1.
#define SYSCALL_PIPE_READ 75    // (pipe_id:int, buf:char*, count:u64) -> bytes leidos o -1 (parcial permitido)
#define SYSCALL_FD_HAS_DATA 76  // (fd:int) -> 1 si hay datos para leer, 0 si vacio, -1 si invalido
#define SYSCALL_PIPE_LIST 77    // (out:pipe_info_t*, max:int) -> cant listada

// Prototipos kernel-side
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
int sys_fd_list_pid(int pid, fd_info_t *out, int max);
int sys_pipe_create(void);
int sys_fd_bind_std(int pid, int which, int pipe_id);
int sys_pipe_write(int pipe_id, const char *buffer, uint64_t count);
int sys_pipe_read(int pipe_id, char *buffer, uint64_t count);
int sys_fd_has_data(int fd);
int sys_pipe_list(pipe_info_t *out, int max);

// Multiplexacion
int sys_poll(int *fds, int count);   // bloquea hasta que alguno tenga datos; retorna cantidad listos
int sys_select(int *fds, int count); // igual interfaz que poll pero puede compactar los listos al inicio (ver implementación)
#endif

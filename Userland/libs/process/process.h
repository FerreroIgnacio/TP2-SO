#ifndef PROCESS_H
#define PROCESS_H

#include "stddef.h"
#include "stdint.h"

typedef enum
{
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2, // default
    PRIORITY_HIGH = 3,
} process_priority_t;

typedef enum
{
    WNOHANG,
    WHANG
} waitpid_options_t;

typedef int (*task_fn_t)(void *);
typedef int pid_t;

typedef struct reg_screenshot
{
    uint64_t rax;    // 0
    uint64_t rbx;    // 1
    uint64_t rcx;    // 2
    uint64_t rdx;    // 3
    uint64_t rsi;    // 4
    uint64_t rdi;    // 5
    uint64_t rbp;    // 6
    uint64_t rsp;    // 7
    uint64_t r8;     // 8
    uint64_t r9;     // 9
    uint64_t r10;    // 10
    uint64_t r11;    // 11
    uint64_t r12;    // 12
    uint64_t r13;    // 13
    uint64_t r14;    // 14
    uint64_t r15;    // 15
    uint64_t rflags; // 16
    uint64_t rip;    // 17
    uint64_t CS;     // 18
    uint64_t SS;     // 19
} reg_screenshot_t;

typedef struct wait_node
{
    int pid;
    struct wait_node *next;
    int status;
} wait_node_t;

typedef struct
{
    int pid; // Identificador de tarea
    int present;
    task_fn_t entryPoint; // Puntero a la función asociada a la tarea
    void *argv;
    int father_pid;

    uint64_t startTime_ticks; // Tiempo transcurrido en ms desde que se encoló (aprox.)

    reg_screenshot_t ctx; // último contexto de registros guardado para este pid (20 qwords)

    int ready;
    int waiting;
    wait_node_t *waiting_node;
    int wait_status;

    int priority;
    int run_tokens; // 1 token = 1 quantum de cpu, a mayor prioridad, mayor tiempo de cpu.

    int was_killed;
    int is_zombie;
    int status;
} proc_info_t;

pid_t new_proc(task_fn_t entry, void *argv);
void exit(int exit_code);
pid_t getpid(void);
int get_proc_list(proc_info_t *out, int max);
int kill(pid_t pid);
process_priority_t get_priority(pid_t pid);
int set_priority(pid_t pid, process_priority_t new_priority);
int block_proc(pid_t pid);
int unblock_proc(pid_t pid);
void yield(void);
pid_t waitpid(pid_t pid, int *status, waitpid_options_t options);

#endif
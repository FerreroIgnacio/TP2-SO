#include "../videoDriver/videoDriver.h"
#include "../syscalls/syscalls.h"
#include "../keyboardDriver/keyboardDriver.h"
#include "../IDT/isrHandlers.h"
#include "../memoryManagement/mm.h"
#include "../scheduler/scheduler.h"
#include "../semaphores/sem.h"
#include "../filesDescriptors/fd.h"
#include "../filesDescriptors/pipes.h"

uint64_t syscallHandler(int syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
    switch (syscall_num)
    {
    case SYSCALL_READ: // read
        return sys_read((int)arg1, (char *)arg2, (uint64_t)arg3);
    case SYSCALL_WRITE: // write
        return sys_write((int)arg1, (const char *)arg2, (uint64_t)arg3);
    case SYSCALL_ISKEYPRESSED:
        return sys_isKeyPressed((int)arg1);
    case SYSCALL_GET_BOOTTIME:
        return sys_getBootTime();
    case SYSCALL_GET_TIME:
        sys_getTime((uint8_t *)arg1, (uint8_t *)arg2, (uint8_t *)arg3);
        return 1;
    case SYSCALL_GET_DATE:
        sys_getDate((uint8_t *)arg1, (uint8_t *)arg2, (uint8_t *)arg3);
        return 1;
    case SYSCALL_GET_REGISTERS:
        sys_getRegisters((uint64_t *)arg1);
        return 1;
    case SYSCALL_GET_FB:
        return (uint64_t)sys_get_fb();
    case SYSCALL_SET_FB:
        sys_set_fb((uint8_t)arg1);
        return 1;
    case SYSCALL_FREE_FB:
        sys_free_fb((uint8_t)arg1);
        return 1;
    case SYSCALL_PLAY_SOUND:
        sys_playSound((uint16_t)arg1);
        return 1;
    case SYSCALL_STOP_SOUND:
        sys_stopSound();
        return 1;
    case SYSCALL_MALLOC:
        return (uint64_t)sys_malloc((uint64_t)arg1);
    case SYSCALL_CALLOC:
        return (uint64_t)sys_calloc((uint64_t)arg1, (uint64_t)arg2);
    case SYSCALL_REALLOC:
        return (uint64_t)sys_realloc((void *)arg1, (uint64_t)arg2);
    case SYSCALL_FREE:
        sys_free((void *)arg1);
        return 1;
    case SYSCALL_GET_MEMORY_INFO:
        sys_get_memory_info((size_t *)arg1, (size_t *)arg2, (size_t *)arg3);
        return 1;
    case SYSCALL_PROC_CREATE: // Iniciar una nueva tarea
        return sys_proc_create((task_fn_t)arg1, (void *)arg2);
    case SYSCALL_PROC_EXIT:
        sys_proc_exit((int)arg1);
        return 1;
    case SYSCALL_GET_PID:
        return sys_get_pid();
    case SYSCALL_PROC_LIST: // Listar tareas
        return sys_proc_list((proc_info_t *)arg1, (int)arg2);
    case SYSCALL_PROC_KILL: // Matar una tarea por pid
        return sys_proc_kill((int)arg1);
    case SYSCALL_SET_PRIORITY:
        return sys_set_priority((int)arg1, (int)arg2);
    case SYSCALL_GET_PRIORITY:
        return sys_get_priority((int)arg1);
    case SYSCALL_BLOCK_PROCESS:
        return sys_block_proc((int)arg1);
    case SYSCALL_UNBLOCK_PROCESS:
        return sys_unblock_proc((int)arg1);
    case SYSCALL_YIELD:
        sys_yield();
        return 1;
    case SYSCALL_WAIT_PID:
        return sys_waitpid((int)arg1, (int *)arg2, (int)arg3);
    case SYSCALL_SLEEP:
        sys_sleep((int)(arg1));
        return 1;
    case SYSCALL_SEM_OPEN:
        return sys_sem_open((const char *)arg1, (int)arg2);
    case SYSCALL_SEM_WAIT:
        return sys_sem_wait((int)arg1);
    case SYSCALL_SEM_POST:
        return sys_sem_post((int)arg1);
    case SYSCALL_SEM_CLOSE:
        return sys_sem_close((int)arg1);
    case SYSCALL_SEM_SET:
        sys_sem_set((int)arg1, (int)arg2);
        return 1;
    case SYSCALL_FD_OPEN: // new fd open
        return sys_fd_open((const char *)arg1);
    case SYSCALL_FD_LIST: // list dynamic FDs
        return sys_fd_list((fd_info_t *)arg1, (int)arg2);
    case SYSCALL_PIPE_CREATE:
        return sys_pipe_create();
    case SYSCALL_FD_BIND_STD:
        return sys_fd_bind_std((int)arg1, (int)arg2, (int)arg3);
    case SYSCALL_PIPE_WRITE:
        return sys_pipe_write((int)arg1, (const char*)arg2, (uint64_t)arg3);
    case SYSCALL_PIPE_READ:
        return sys_pipe_read((int)arg1, (char*)arg2, (uint64_t)arg3);
    default:
        return -1;
    }
}

/*
 * ID 0
 */
int sys_read(int fd, char *buffer, uint64_t count)
{
    if (buffer == 0 || count == 0)
        return 0;
    return fd_read(fd, buffer, count); // all FDs uniforme
}

/*
 * ID 1
 */
int sys_write(int fd, const char *buffer, uint64_t count)
{
    if (buffer == 0 || count == 0)
        return 0;
    // All descriptors treated uniformly; blocking handled in fd_write
    return fd_write(fd, buffer, count);
}

/*
 * ID 2
 */
int sys_isKeyPressed(uint16_t makecode)
{
    return isKeyPressed(makecode);
}

/*
 * ID 3
 */
uint64_t sys_getBootTime()
{
    return getSysTicks() * 55;
}

/*
 * ID 7
 */
frameBuffer_adt sys_get_fb()
{
    return getFB();
}

/*
 * ID 8
 */
void sys_set_fb(uint8_t index)
{
    setFB(index);
}

/*
 * ID 9
 */
void sys_free_fb(uint8_t index)
{
    freeFB(index);
}

/*
 * ID 30
 */
void *sys_malloc(uint64_t size)
{
    return mm_malloc((size_t)size);
}

/*
 * ID 31
 */
void *sys_calloc(uint64_t count, uint64_t size)
{
    return mm_calloc((size_t)count, (size_t)size);
}

/*
 * ID 32
 */
void *sys_realloc(void *ptr, uint64_t size)
{
    return mm_realloc(ptr, (size_t)size);
}

/*
 * ID 33
 */
void sys_free(void *ptr)
{
    mm_free(ptr);
}

/*
 * ID 34
 */
void sys_get_memory_info(size_t *total, size_t *used, size_t *free)
{
    size_t total_memory, used_memory;
    mm_get_memory_info(&total_memory, &used_memory);
    *total = total_memory;
    *used = used_memory;
    *free = total_memory - used_memory;
}

/*
 * ID 40
 */
// Inicia  una tarea; devuelve el pid asignado o -1 si falla.
int sys_proc_create(task_fn_t entry, void *argv)
{
    if (entry == 0)
        return -1;
    return scheduler_add(entry, argv);
}

/*
 * ID 41
 */
void sys_proc_exit(int exit_code)
{
    scheduler_exit(exit_code);
}

/*
 * ID 42
 */
int sys_get_pid(void)
{
    return scheduler_current_pid();
}

/*
 * ID 43
 */
// Lista tareas actualmente en cola; devuelve la cantidad copiada en 'out'.
int sys_proc_list(proc_info_t *out, int max)
{
    return scheduler_list(out, max);
}

/*
 * ID 44
 */
// Mata una tarea por pid; devuelve 0 si tuvo éxito, -1 si error.
int sys_proc_kill(int pid)
{
    return scheduler_kill(pid);
}
/*
 * ID 45
 */
int sys_set_priority(int pid, int new_priority)
{
    return scheduler_set_priority(pid, new_priority);
}

/*
 * ID 46
 */
int sys_get_priority(int pid)
{
    return scheduler_get_priority(pid);
}

/*
 * ID 47
 */
int sys_block_proc(int pid)
{
    return scheduler_block_pid(pid);
}

/*
 * ID 48
 */
int sys_unblock_proc(int pid)
{
    return scheduler_unblock_pid(pid);
}
/*
 * ID 49
 */
void sys_yield(void)
{
    scheduler_yield();
}

/*
 * ID 50
 */
int sys_waitpid(int pid, int *status, int options)
{
    return scheduler_wait_pid(pid, status, options);
}

/*
 * ID 51
 */
void sys_sleep(int ms)
{
    scheduler_sleep(ms);
}

int sys_sem_open(const char *name, int initial_value)
{
    return sem_open(name, initial_value);
}

int sys_sem_wait(int sem_id)
{
    return sem_wait(sem_id);
}

int sys_sem_post(int sem_id)
{
    return sem_post(sem_id);
}

int sys_sem_close(int sem_id)
{
    return sem_close(sem_id);
}
void sys_sem_set(int sem_id, int new_value)
{
    sem_set(sem_id, new_value);
}

int sys_fd_open(const char *name)
{
    return fd_create(name);
}

int sys_fd_list(fd_info_t *out, int max)
{
    return fd_list(out, max);
}

int sys_pipe_create(void)
{
    return pipe_create();
}

int sys_fd_bind_std(int pid, int which, int pipe_id)
{
    return fd_bind_std_for_pid(pid, which, pipe_id);
}
int sys_pipe_write(int pipe_id, const char *buffer, uint64_t count){
    if(buffer==0 || count==0) return 0;
    return pipe_write(pipe_id, buffer, count);
}
int sys_pipe_read(int pipe_id, char *buffer, uint64_t count){
    if(buffer==0 || count==0) return 0;
    return pipe_read(pipe_id, buffer, count);
}


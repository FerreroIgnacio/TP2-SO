#include "scheduler.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "../IDT/isrHandlers.h"   // Para medir tiempo por ticks
#include "../syscalls/syscalls.h" // saveRegisters / sys_getRegisters
#include "../semaphores/sem.h"

#ifndef MAX_TASKS
#define MAX_TASKS 16
#endif

#define TASK_STACK_SIZE (16 * 1024)
static uint8_t task_stacks[MAX_TASKS][TASK_STACK_SIZE];
static inline uint64_t top_of_stack(int pid)
{
    uint64_t top = (uint64_t)&task_stacks[pid][TASK_STACK_SIZE];
    return top & ~((uint64_t)0xF);
}

static inline uint16_t read_cs(void)
{
    uint16_t v;
    __asm__ volatile("mov %%cs, %0" : "=r"(v));
    return v;
}
static inline uint16_t read_ss(void)
{
    uint16_t v;
    __asm__ volatile("mov %%ss, %0" : "=r"(v));
    return v;
}
static inline uint64_t read_rflags(void)
{
    uint64_t v;
    __asm__ volatile("pushfq; pop %0" : "=r"(v));
    return v;
}

// Cola de procesos y su info
static proc_info_t procQueue[MAX_TASKS] = {0};

// Tarea init/idle que se ejecuta cuando no hay tareas listas.
static int default_idle(void *argv);
static task_fn_t init_task_fn = default_idle;
static void *init_task_argv = NULL;
static int current_pid = -1;

static bool is_valid_pid(int pid)
{
    if (pid < 0 || pid >= MAX_TASKS)
    {
        return false;
    }
    proc_info_t *proc = &procQueue[pid];
    return proc->present;
}

/*
 * default_idle
 * Se invoca cuando no hay tareas listas para correr.
 * Detiene la CPU una vez (hasta la próxima interrupción) y retorna.
 * No se llama directamente: se establece con scheduler_set_idle o se usa por defecto.
 */
static int default_idle(void *argv)
{
    cpu_halt();
    return 0;
}

/*
 * scheduler_set_idle
 * Configura la tarea init/idle usada cuando no hay tareas listas.
 * Uso: Personalización opcional después de scheduler_init; pasar NULL restaura el idle por defecto.
 */
void scheduler_set_idle(task_fn_t idle_task, void *argv)
{
    init_task_fn = (idle_task != NULL) ? idle_task : default_idle;
    init_task_argv = argv;
}

/*
 * scheduler_add
 * Encola una tarea para ser ejecutada.
 * Retorna: PID (índice del arreglo) si tiene éxito, -1 si la cola está llena o la tarea es NULL.
 * Uso: int pid = scheduler_add(mi_tarea);
 */
int scheduler_add(task_fn_t task, void *argv)
{
    if (task == NULL)
        return -1;
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (procQueue[i].present == false)
        {
            procQueue[i] = (proc_info_t){
                .pid = i,
                .present = true,
                .entryPoint = task,
                .argv = argv,
                .father_pid = current_pid,
                .startTime_ticks = getSysTicks(),
                .ctx = {0},
                .ready = true,
                .waiting = false,
                .waiting_node = NULL,
                .wait_status = 0,
                .priority = PRIORITY_NORMAL,
                .run_tokens = PRIORITY_NORMAL,
                .was_killed = false,
                .is_zombie = false,
                .status = 0};
            return i;
        }
    }
    return -1; // cola llena
}

/*
 * scheduler_kill
 * Elimina una tarea encolada (por PID) para que no se ejecute.
 * Nota: Este scheduler cooperativo simple no desaloja una tarea en ejecución; kill solo afecta tareas en cola (no las que se están ejecutando).
 * Retorna: 0 si tuvo éxito; -1 si el PID es inválido o no hay tarea en ese PID.
 * Uso: scheduler_kill(pid);
 */

int scheduler_kill(int pid)
{
    if (pid < 0 || pid >= MAX_TASKS || procQueue[pid].present == false)
        return -1;
    procQueue[pid].is_zombie = true;
    procQueue[pid].was_killed = true;

    if (current_pid == pid)
    {
        procQueue[pid].run_tokens = 0;
        scheduler_switch(NULL);
    }
    return 0;
}

void scheduler_exit(int status)
{
    int pid = current_pid;
    if (pid < 0 || pid >= MAX_TASKS || procQueue[pid].present == false)
        return;
    procQueue[pid].is_zombie = true;
    procQueue[pid].status = status;
    procQueue[pid].run_tokens = 0;
    scheduler_switch(NULL);
}

void scheduler_yield()
{
    scheduler_save_and_switch();
}

/*
 * scheduler_list
 * Copia hasta 'max' tareas actualmente encoladas en 'out' con su pid, puntero de entrada y runtime aprox en ms.
 * Retorna la cantidad copiada.
 */
int scheduler_list(proc_info_t *out, int max)
{
    if (out == NULL || max <= 0)
        return 0;
    int count = 0;
    for (int i = 0; i < MAX_TASKS && count < max; i++)
    {
        if (procQueue[i].present == true)
        {
            out[count] = procQueue[i];
            count++;
        }
    }
    return count;
}

/*
 * Loopea en el proc arr y devuelve el proximo proc ready o -1 si queda en idle
 */
static int find_next_ready_from(int start_exclusive)
{
    for (int step = 1; step <= MAX_TASKS; step++)
    {
        int idx = (start_exclusive + step) % MAX_TASKS;
        if (procQueue[idx].present == true && procQueue[idx].ready && !procQueue[idx].is_zombie)
        {
            return idx;
        }
    }
    // no se encontraron procesos ready
    return -1;
}

void scheduler_switch(reg_screenshot_t *regs)
{

    if (procQueue[current_pid].run_tokens > 0)
    {
        --procQueue[current_pid].run_tokens;
        interrupt_setRegisters(regs);
    }

    // Elegir próximo candidato en round-robin
    int idx = find_next_ready_from(current_pid);
    if (idx < 0 || idx >= MAX_TASKS)
    {
        (void)init_task_fn(init_task_argv);
        return;
    }

    if (regs != NULL)
    {
        if (current_pid >= 0 && current_pid < MAX_TASKS && procQueue[current_pid].present)
        {
            memcpy(&procQueue[current_pid].ctx, regs, sizeof(reg_screenshot_t));
        }

        proc_info_t *next = &procQueue[idx];
        current_pid = idx;
        next->run_tokens = next->priority;

        if (next->ctx.rip == 0)
        {
            reg_screenshot_t *ctx = &next->ctx;
            *ctx = *regs; // base desde snapshot actual
            ctx->rip = (uint64_t)next->entryPoint;
            ctx->rdi = (uint64_t)next->argv;
            ctx->rsp = top_of_stack(idx);
            ctx->rbp = ctx->rsp;
            ctx->rflags |= (1ULL << 9); // IF=1
        }

        interrupt_setRegisters(&procQueue[current_pid].ctx);
        return;
    }

    // Camino cooperativo: bootstrap y saltar con iretq si es primera vez
    proc_info_t *next = &procQueue[idx];
    current_pid = idx;
    next->run_tokens = next->priority;
    if (next->ctx.rip == 0)
    {
        reg_screenshot_t *ctx = &next->ctx;
        memset(ctx, 0, sizeof(*ctx));
        ctx->rip = (uint64_t)next->entryPoint;
        ctx->rdi = (uint64_t)next->argv;
        ctx->rsp = top_of_stack(idx);
        ctx->rbp = ctx->rsp;
        ctx->rflags = read_rflags() | (1ULL << 9); // IF=1
        ctx->CS = read_cs();
        ctx->SS = read_ss();
        interrupt_setRegisters(ctx);
        return;
    }

    // Reanudar tarea ya iniciada
    interrupt_setRegisters(&procQueue[current_pid].ctx);
}

int scheduler_current_pid(void)
{
    return current_pid;
}

int scheduler_block_current(struct wait_node *wait_token)
{
    if (current_pid < 0 || wait_token == NULL)
    {
        return -1;
    }
    proc_info_t *proc = &procQueue[current_pid];
    proc->ready = 0;
    proc->waiting = 1;
    proc->waiting_node = wait_token;
    proc->wait_status = -1;

    scheduler_save_and_switch();

    int status = proc->wait_status;
    proc->waiting = 0;
    proc->waiting_node = NULL;
    proc->wait_status = 0;
    return status;
}

void scheduler_unblock(int pid, struct wait_node *wait_token, int status)
{
    if (!is_valid_pid(pid) || wait_token == NULL)
    {
        return;
    }
    proc_info_t *proc = &procQueue[pid];
    if (!proc->waiting || proc->waiting_node != wait_token)
    {
        return;
    }
    proc->wait_status = status;
    proc->ready = 1;
    proc->waiting = 0;
    proc->waiting_node = NULL;
    proc->run_tokens = proc->priority;
}

int scheduler_block_pid(int pid)
{
    if (!is_valid_pid(pid))
    {
        return -1;
    }

    proc_info_t *proc = &procQueue[pid];
    if (!proc->ready)
    {
        return -1;
    }

    if (proc->waiting)
    {
        return -1;
    }

    proc->ready = 0;
    proc->waiting = 0;
    proc->waiting_node = NULL;
    proc->wait_status = 0;
    proc->run_tokens = 0;

    if (pid == current_pid)
    {
        scheduler_save_and_switch();
    }

    return 0;
}

int scheduler_unblock_pid(int pid)
{
    if (!is_valid_pid(pid))
    {
        return -1;
    }

    proc_info_t *proc = &procQueue[pid];
    if (proc->ready || proc->waiting)
    {
        return -1;
    }

    proc->ready = 1;
    proc->run_tokens = proc->priority;
    return 0;
}

int scheduler_set_priority(int pid, process_priority_t new_priority)
{
    if (pid < 0 || pid >= MAX_TASKS || new_priority < PRIORITY_LOW || new_priority > PRIORITY_HIGH || procQueue[pid].present == false)
    {
        return -1;
    }
    procQueue[pid].priority = new_priority;
    return 0;
}

process_priority_t scheduler_get_priority(int pid)
{
    if (pid < 0 || pid >= MAX_TASKS || procQueue[pid].present == false)
    {
        return -1;
    }
    return procQueue[pid].priority;
}

void scheduler_start(void)
{
    while (1)
    {
        int idx = find_next_ready_from(current_pid);

        if (idx >= 0 && idx < MAX_TASKS)
        {
            proc_info_t *next = &procQueue[idx];
            current_pid = idx;
            if (next->ctx.rip == 0)
            {
                reg_screenshot_t *ctx = &next->ctx;
                memset(ctx, 0, sizeof(*ctx));
                ctx->rip = (uint64_t)next->entryPoint;
                ctx->rdi = (uint64_t)next->argv;
                ctx->rsp = top_of_stack(idx);
                ctx->rbp = ctx->rsp;
                ctx->rflags = read_rflags() | (1ULL << 9);
                ctx->CS = read_cs();
                ctx->SS = read_ss();
                interrupt_setRegisters(ctx);
            }
            else
            {
                interrupt_setRegisters(&next->ctx);
            }
        }
        else
        {
            (void)init_task_fn(init_task_argv);
        }
    }
}

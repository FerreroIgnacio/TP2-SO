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

// Cola de procesos y su info
static proc_info_t procQueue[MAX_TASKS] = {0};

// Tarea init/idle que se ejecuta cuando no hay tareas listas.
static int default_idle(void *argv);
static task_fn_t init_task_fn = default_idle;
static void *init_task_argv = NULL;
// Pid actualmente en ejecución (o -1 si idle/ninguna) mientras se ejecuta una tarea en t().
static int current_pid = -1;
// Índice base para round-robin (siguiente candidato a ejecutar)
static int next_index = 0;

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
 * start_task
 * Inicia la ejecución de la tarea en procQueue[idx] desde su entrypoint.
 */
static void start_task(int idx)
{
    if (idx < 0 || idx >= MAX_TASKS)
        return;
    current_pid = idx;
    task_fn_t t = procQueue[idx].entryPoint;
    // si retorna, terminó, se guarda el status.
    procQueue[idx].status = (int)t(procQueue[idx].argv);
    procQueue[idx].is_zombie = 1;
    current_pid = -1;
    next_index = (idx + 1) % MAX_TASKS;
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
        if (procQueue[i].entryPoint == NULL)
        {
            procQueue[i] = (proc_info_t){
                .pid = i,
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

    if (pid < 0 || pid >= MAX_TASKS)
        return -1;
    if (procQueue[pid].entryPoint == NULL)
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
        if (procQueue[i].entryPoint != NULL)
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
        if (procQueue[idx].entryPoint != NULL && procQueue[idx].ready && !procQueue[idx].is_zombie)
        {
            return idx;
        }
    }
    // no se encontraron procesos ready
    return -1;
}

void scheduler_switch(reg_screenshot_t *regs)
{
    if (--procQueue[current_pid].run_tokens > 0)
    {
        interrupt_setRegisters(regs);
    }

    int idx = find_next_ready_from(next_index - 1);
    if (idx < 0)
    {
        // No hay procesos listos: permanecer en idle
        current_pid = -1;
        return;
    }
    if (regs != NULL)
    {
        memcpy(&procQueue[current_pid].ctx, regs, sizeof(reg_screenshot_t));
    }
    current_pid = idx;
    next_index = (idx + 1) % MAX_TASKS;

    if (procQueue[current_pid].ctx.rip != 0)
    {
        procQueue[current_pid].run_tokens = procQueue[current_pid].priority;
        interrupt_setRegisters(&procQueue[current_pid].ctx);
    }
    else
    {
        start_task(current_pid);
    }
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
    if (pid < 0 || pid >= MAX_TASKS)
    {
        return;
    }
    proc_info_t *proc = &procQueue[pid];
    if (proc->entryPoint == NULL)
    {
        return;
    }
    proc->wait_status = status;
    proc->ready = 1;
    proc->waiting = 0;
    proc->waiting_node = wait_token;
    // TODO: Ensure unblocked task gets scheduled soon (e.g., enqueue in ready list).
}

int scheduler_set_priority(int pid, process_priority_t new_priority)
{
    if (new_priority < PRIORITY_LOW || new_priority > PRIORITY_HIGH || procQueue[pid].entryPoint == NULL)
    {
        return -1;
    }
    procQueue[pid].priority = new_priority;
    return 0;
}

process_priority_t scheduler_get_priority(int pid)
{
    if (procQueue[pid].entryPoint != NULL)
    {
        return procQueue[pid].priority;
    }
    return -1;
}

void scheduler_start(void)
{
    // TODO: implementar proceso génesis
    while (1)
    {
        // Buscar a partir de next_index (round-robin)
        int idx = find_next_ready_from((next_index - 1) > 0 ? (next_index - 1) : 0);

        if (idx >= 0)
        {
            start_task(idx);
        }
        else
        {
            // No hay tareas para correr: ejecutar init/idle
            if (init_task_fn)
            {
                (void)init_task_fn(init_task_argv);
            }
            else
            {
                (void)default_idle(NULL);
            }
        }
    }
}

#include <scheduler.h>
#include <stddef.h>
#include <stdbool.h>
#include <cpu.h>
#include <isrHandlers.h> // Para medir tiempo por ticks
#include <stdint.h>
#include <syscalls.h>    // saveRegisters / sys_getRegisters
#include <sem.h>

#ifndef MAX_TASKS
#define MAX_TASKS 16
#endif

// Cola de procesos y su info
static proc_info_t procQueue[MAX_TASKS] = {0};

// Tarea init/idle que se ejecuta cuando no hay tareas listas.
static int default_idle(void);
static task_fn_t init_task_fn = default_idle;
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
static int default_idle(void) {
    cpu_halt();
    return 0;
}

/*
 * scheduler_set_idle
 * Configura la tarea init/idle usada cuando no hay tareas listas.
 * Uso: Personalización opcional después de scheduler_init; pasar NULL restaura el idle por defecto.
 */
void scheduler_set_idle(task_fn_t idle_task) {
    init_task_fn = (idle_task != NULL) ? idle_task : default_idle;
}

/*
 * scheduler_add
 * Encola una tarea para ser ejecutada.
 * Retorna: PID (índice del arreglo) si tiene éxito, -1 si la cola está llena o la tarea es NULL.
 * Uso: int pid = scheduler_add(mi_tarea);
 */
int scheduler_add(task_fn_t task) {
    if (task == NULL) return -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (procQueue[i].entryPoint == NULL) {
            procQueue[i] = (proc_info_t){
                .pid = i,
                .entryPoint = task,
                .currentPoint = task,
                .startTime_ticks = getSysTicks(),
                .ready = 1,
                .waiting = 0,
                .waiting_node = NULL,
                .wait_status = 0,
            };
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

int scheduler_kill(int pid) {

    if (pid < 0 || pid >= MAX_TASKS) return -1;
    if (procQueue[pid].entryPoint == NULL) return -1;
    sem_cleanup_dead_process(pid);
    procQueue[pid] = (proc_info_t){0};
    // Si matamos el que estaba "seleccionado", adelantar el puntero
    if (current_pid == pid) {
        current_pid = -1;
    }
    return 0;
}

/*
 * scheduler_list
 * Copia hasta 'max' tareas actualmente encoladas en 'out' con su pid, puntero de entrada y runtime aprox en ms.
 * Retorna la cantidad copiada.
 */
int scheduler_list(proc_info_t* out, int max) {
    if (out == NULL || max <= 0) return 0;
    int count = 0;
    for (int i = 0; i < MAX_TASKS && count < max; i++) {
        if (procQueue[i].entryPoint != NULL) {
            out[count] = procQueue[i];
            count++;
        }
    }
    return count;
}

/*
 * Loopea en el proc arr y devuelve el proximo proc ready o -1 si queda en idle
 */
static int find_next_ready_from(int start_exclusive) {
    for (int step = 1; step <= MAX_TASKS; step++) {
        int idx = (start_exclusive + step) % MAX_TASKS;
        if (procQueue[idx].entryPoint != NULL && procQueue[idx].ready) {
            return idx;
        }
    }
    //no se encontraron procesos ready
    return -1;
}

void scheduler_switch(void) {
    // TODO: Implement cooperative context switch so blocked tasks yield CPU.
}

int scheduler_current_pid(void) {
    return current_pid;
}

int scheduler_block_current(struct wait_node *wait_token) {
    if (current_pid < 0 || wait_token == NULL) {
        return -1;
    }
    proc_info_t *proc = &procQueue[current_pid];
    proc->ready = 0;
    proc->waiting = 1;
    proc->waiting_node = wait_token;
    proc->wait_status = -1;
    // TODO: After saving current context, transfer control to next ready task here.
    scheduler_switch();
    int status = proc->wait_status;
    proc->waiting = 0;
    proc->waiting_node = NULL;
    proc->wait_status = 0;
    return status;
}

void scheduler_unblock(int pid, struct wait_node *wait_token, int status) {
    if (pid < 0 || pid >= MAX_TASKS) {
        return;
    }
    proc_info_t *proc = &procQueue[pid];
    if (proc->entryPoint == NULL) {
        return;
    }
    proc->wait_status = status;
    proc->ready = 1;
    proc->waiting = 0;
    proc->waiting_node = wait_token;
    // TODO: Ensure unblocked task gets scheduled soon (e.g., enqueue in ready list).
}

void scheduler_start(void) {
    while (1) {
        // Buscar a partir de next_index (round-robin)
        int idx = find_next_ready_from((next_index - 1) > 0 ? (next_index - 1) : 0);

        if (idx >= 0) {
            current_pid = idx;
            task_fn_t t = procQueue[idx].entryPoint;
            // Ejecutar tarea en modo cooperativo: si retorna, se remueve de la cola
            (void)t();
            sem_cleanup_dead_process(idx);
            procQueue[idx] = (proc_info_t){0};
            current_pid = -1;
            // Avanzar el puntero para continuar el round-robin
            next_index = (idx + 1) % MAX_TASKS;
        } else {
            // No hay tareas para correr: ejecutar init/idle
            if (init_task_fn) {
                (void)init_task_fn();
            } else {
                (void)default_idle();
            }
        }
    }
}

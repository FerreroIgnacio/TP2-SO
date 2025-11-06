#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <registerManagement.h>

struct wait_node;

// Firma básica de una tarea: no recibe argumentos y devuelve un int.
// El valor de retorno actualmente es ignorado por el scheduler.
typedef int (*task_fn_t)(void);

// Información mínima de un proceso/tarea expuesta para listados.
typedef struct
{
    int pid;                  // Identificador de tarea
    task_fn_t entryPoint;     // Puntero a la función asociada a la tarea
    uint64_t startTime_ticks; // Tiempo transcurrido en ms desde que se encoló (aprox.)
    reg_screenshot_t ctx;     // último contexto de registros guardado para este pid (18 qwords)
    int ready;
    int waiting;
    struct wait_node *waiting_node;
    int wait_status;
} proc_info_t;

/*
 * Configura la tarea init/idle que se ejecuta cuando no hay tareas listas.
 * Si se pasa NULL, se usa una tarea idle por defecto que detiene la CPU una vez.
 * Uso: Opcional. Llamar después de scheduler_init para personalizar el idle.
 */
void scheduler_set_idle(task_fn_t idle_task);

/*
 * Agrega una tarea a la cola de ejecución.
 * Devuelve un id de tarea (pid) en [0..N) si tiene éxito, o -1 si la cola está llena/inválida.
 * Uso: scheduler_add(mi_tarea);
 */
int scheduler_add(task_fn_t task);

/*
 * Elimina una tarea de la cola a partir de su pid (id de tarea).
 * Devuelve 0 si tuvo éxito, -1 si el pid es inválido o ya está vacío.
 * Uso: scheduler_kill(pid);
 */
int scheduler_kill(int pid);

/*
 * Inicia el bucle de planificación cooperativa. No retorna en operación normal.
 * Ejecuta las tareas en cola hasta que terminen (retornen). Si no hay tareas,
 * corre la tarea init/idle que detiene la CPU una vez y retorna.
 * Uso: Llamar una vez tras la inicialización y el registro de tareas.
 */
void scheduler_start(void);

/*
 * Llena un buffer con la lista de tareas actualmente encoladas para ejecutar.
 * Retorna la cantidad de entradas copiadas (<= max).
 * Nota: Tiempo aproximado desde que se encoló la tarea (no CPU time real en este scheduler cooperativo).
 */
int scheduler_list(proc_info_t *out, int max);

// Avanza el puntero del scheduler al próximo proceso listo (round-robin simple).
void scheduler_switch(reg_screenshot_t *regs);

// Identificador del proceso actualmente en ejecución o -1 si idle.
int scheduler_current_pid(void);

// Bloquea el proceso en ejecución y entrega control al scheduler. Retorna el
// estado con el que fue desbloqueado (0 éxito, negativo error).
int scheduler_block_current(struct wait_node *wait_token);

// Marca un proceso bloqueado como listo nuevamente, definiendo el estado con el
// que despertará.
void scheduler_unblock(int pid, struct wait_node *wait_token, int status);

#endif // SCHEDULER_H

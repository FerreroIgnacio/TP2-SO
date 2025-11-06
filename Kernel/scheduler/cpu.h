#ifndef CPU_H
#define CPU_H

/*
 * cpu_halt
 * Detiene la CPU hasta la próxima interrupción externa.
 * Uso: Llamar desde contextos de inactividad (idle) para evitar busy-wait.
 */
void cpu_halt(void);

#endif // CPU_H

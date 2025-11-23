#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include "../../libs/process/process.h"

// Tipo de función para ejecutar comandos ya parseados
typedef int (*shell_cmd_run_fn)(int argc, char **argv);

// Definición de un comando de la shell
typedef struct
{
    const char *name;  // nombre del comando (token)
    int minArgsCount;  // cantidad mínima de argumentos requeridos (sin contar el nombre)
    int maxArgsCount;  // cantidad máxima de argumentos soportados (puede ser igual a min para fijo). -1 => ilimitado
    const char *help;  // descripción corta que aparece en listado de help
    const char *usage; // string de uso (ej: "mem" / "kill <pid>")
    int inbuilt;       // 1 => comando interno (ejecutar directamente en la shell), 0 => lanzar como nuevo proceso
    task_fn_t run;     // función que ejecuta el comando
} shell_cmd_t;

// Ejecuta el comando indicado con la línea ya separada en cmd y args (string crudo de args)
int command_switch(char **line);

// Manejo de proceso en foreground
pid_t get_foreground_proc();
void set_foreground_proc(pid_t pid);

// Pipe de salida del proceso en foreground (si existe)
int get_foreground_pipe_fd();
void flush_foreground_output();

// Utilidad para lanzar funciones como procesos en foreground (bloquea shell hasta terminar)
void run_in_foreground(task_fn_t fn, void *arg);

// Registro global de comandos
extern shell_cmd_t shell_commands[]; // tabla de comandos
extern int shell_commands_count;     // cantidad de entradas válidas

// Algunos comandos son usados externamente
void cmd_echo(char *args); // mantiene interfaz previa para compatibilidad

#endif
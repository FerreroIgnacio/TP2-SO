#include "./shell_commands.h"
#include "../../libs/standard/standard.h"
#include "../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../libs/process/process.h"
#include "../../libs/memory/memory.h"
#include "../../libs/mystring/mystring.h"
#include "./shell_defs.h"
#include "./shell_render.h"
#include "./commands/commands.h"

#define MAX_PROCESS 100
#define ARGV_MAX 64 // máximo 64 argumentos por comando

// Estado del proceso foreground
pid_t foreground_proc_running = 1;
static int foreground_pipe_fd = -1;

// Implementación para ejecutar una función como proceso en foreground
void run_in_foreground(task_fn_t fn, void *arg){
    pid_t pid = new_proc(fn, arg);
    fprintf(3, "creado proc con id %d\n", pid);
  //  block_proc(pid);
    int pipe_id = pipe_create();
    if (pid <= 0 || pipe_id == -1){return;}
    foreground_pipe_fd = pipe_id;
    fd_bind_std(pid, STDIN, 5);
    fd_bind_std(pid, STDOUT, 6);
    fd_bind_std(getpid(), 3, 6); // mantener stdin shell

    //unblock_proc(pid);

   // waitpid(pid);
  // flush_foreground_output();
    cmd_pipelist_run(0, 0);
    set_foreground_proc(pid);
    shell_print_colored("Proceso foreground finalizado.\n", 0x00FF00);
    fprintf(3, "pipe3HasData:%d ; pipe6hasData:%d",fd_has_data(3), fd_has_data(6));

    fprintf(3, "hello");
    consume_render_fd();
}
int get_foreground_pipe_fd(){ return foreground_pipe_fd; }
void flush_foreground_output(){
    if (foreground_pipe_fd < 0) return;
    unsigned char buf[256];
    while (fd_has_data(foreground_pipe_fd)){
        int n = read(foreground_pipe_fd, buf, sizeof(buf)-1);
        if (n <= 0) break;
        buf[n] = '\0';
        // Escribir al buffer renderizado (FD 3)
        fprintf(3, "%s", (char*)buf);
        consume_render_fd();
    }
}

pid_t get_foreground_proc(){ return foreground_proc_running; }
void set_foreground_proc(pid_t pid){ foreground_proc_running = pid; }

// Tabla de comandos centralizada
shell_cmd_t shell_commands[] = {
    {"help", 0, 0, "Mostrar comandos disponibles", "help", 0, cmd_help_run},
    {"clear", 0, 0, "Limpiar pantalla", "clear", 1, cmd_clear_run},
    {"mem", 0, 0, "Imprime el estado de la memoria", "mem", 0, cmd_mem_run},
    {"ps", 0, 0, "Imprime la lista de todos los procesos", "ps", 0, cmd_ps_run},
    {"loop", 1, 1, "Imprime su ID cada N segundos", "loop <segundos>", 0, cmd_loop_run},
    {"kill", 1, 1, "Mata un proceso dado su ID", "kill <pid>", 0, cmd_kill_run},
    {"nice", 2, 2, "Cambia la prioridad de un proceso", "nice <pid> <pri>", 0, cmd_nice_run},
    {"block", 1, 1, "Alterna ready/blocked de un proceso", "block <pid>", 0, cmd_block_run},
    {"cat", 0, 0, "Imprime stdin tal como lo recibe", "cat", 0, cmd_cat_run},
    {"wc", 0, 0, "Cuenta cantidad de lineas del input", "wc", 0, cmd_wc_run},
    {"filter", 0, 0, "Filtra las vocales del input", "filter", 0, cmd_filter_run},
    {"mvar", 0, 0, "Problema múltiples lectores (demo)", "mvar", 0, cmd_mvar_run},
    {"createfd", 1, 1, "Crea un FD dinamico", "createfd <name>", 0, cmd_createfd_run},
    {"writefd", 2, -1, "Escribe texto en FD dinamico", "writefd <fd> <texto>", 0, cmd_writefd_run},
    {"readfd", 1, 1, "Lee contenido de un FD dinamico", "readfd <fd>", 0, cmd_readfd_run},
    {"fdlist", 0, 0, "Lista FDs dinamicos del proceso", "fdlist", 1, cmd_fdlist_run},
    {"pipelist", 0, 0, "Lista pipes de kernel (id, uso, size, colas)", "pipelist", 1, cmd_pipelist_run},
    {"test_mm", 1, 1, "Stress test manejador memoria", "test_mm <max-bytes>", 0, cmd_test_mm_run},
    {"test_processes", 1, 1, "Test de creación/bloqueo procesos", "test_processes <max-processes>", 0, cmd_test_processes_run},
    {"test_priority", 1, 1, "Test prioridades de procesos", "test_priority <end-val>", 0, cmd_test_priority_run},
    {"test_synchro", 2, 2, "Test concurrencia con semáforos", "test_synchro <val> <inc-dec>", 0, cmd_test_synchro_run},
    {"test_no_synchro", 2, 2, "Test concurrencia sin semáforos", "test_no_synchro <val> <inc-dec>", 0, cmd_test_no_synchro_run},
    {"echo", 0, -1, "Echo simple", "echo <texto>", 0, cmd_echo_run},
    {"datetime", 0, 0, "Fecha y hora UTC-0", "datetime", 0, cmd_datetime_run},
    {"registers", 0, 0, "Mostrar registros guardados", "registers", 0, cmd_registers_run},
    {"testzerodiv", 0, 0, "Forzar excepcion 00", "testzerodiv", 1, cmd_testzerodiv_run},
    {"testinvalidcode", 0, 0, "Forzar excepcion 06", "testinvalidcode", 1, cmd_testinvalidcode_run},
    {"listfonts", 0, 0, "Listar fuentes disponibles", "listfonts", 1, cmd_listfonts_run},
    {"setfont", 1, 1, "Cambiar fuente", "setfont <id>", 1, cmd_setfont_run},
    {"pongisgolf", 0, 0, "Ejecutar juego pongisgolf", "pongisgolf", 0, cmd_pong_run},
};
int shell_commands_count = sizeof(shell_commands)/sizeof(shell_commands[0]);

// Parser simple de argumentos (split por espacios consecutivos)
static int parse_args(char *args, char **out){
    int count = 0;
    if (!args) return 0;
    while (*args == ' ') args++;
    while (*args){
        if (count >= ARGV_MAX) break;
        out[count++] = args;
        while (*args && *args != ' ') args++;
        if (!*args) break;
        *args = '\0';
        args++;
        while (*args == ' ') args++;
    }
    return count;
}

int command_switch(char *cmd_copy, char *args) {

        for (int i = 0; i < shell_commands_count; i++) {
            shell_cmd_t *c = &shell_commands[i];
            if (!strcmp(cmd_copy, c->name)) {
                char *argv_raw = args;
                char *argv_array[ARGV_MAX];
                int argc = parse_args(argv_raw, argv_array);
                if ((argc < c->minArgsCount) || (c->maxArgsCount != -1 && argc > c->maxArgsCount)) {
                    shell_print_colored("Uso: ", ERROR_COLOR);
                    fprintf(3, "%s\n", c->usage);
                    consume_render_fd();
                    shell_newline();
                    return 0;
                }
                if (!c->inbuilt) {
                    run_in_foreground((task_fn_t) cmd_help_run, NULL);
                } else {
                    int code = c->run(argc, argv_array);
                    if (code) {
                        fprintf(3, "Comando desconocido '%s'\nEscribe 'help' para ver comandos disponibles.\n",
                                cmd_copy);
                        consume_render_fd();
                    }
                }
                shell_newline();
                return 0;

            }
        }
        return -1;
}

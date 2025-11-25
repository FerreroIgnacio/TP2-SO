#include "./shell_commands.h"
#include "../../libs/standard/standard.h"
#include "../../libs/video/video.h"
#include "../../libs/fontManager/fontManager.h"
#include "../../libs/invalidOpCode/invalidOpCode.h"
#include "../../libs/test/test.h"
#include "../../libs/mystring/mystring.h"
#include "../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../libs/memory/memory.h"
#include "../../libs/keyboard/keyboard.h"
#include "../../libs/time/time.h"
#include "../../libs/process/process.h"
#include "../../libs/semaphores/semaphores.h"
#include "./shell_defs.h"
#include "./shell_render.h"
#include <stdarg.h>

extern int get_shell_stdout_pipe();

// Comandos disponibles
static int cmd_help()
{
    printf("Comandos disponibles:\n");
    printf("  help             - Mostrar comandos disponibles\n");                                          // OK
    printf("  clear            - Limpiar pantalla\n");                                                      // OK
    printf("  mem              - Imprime el estado de la memoria\n");                                       // OK
    printf("  ps               - Imprime la lista de todos los procesos\n");                                // OK
    printf("  loop <segundos>  - Imprime su ID con un saludo cada una determinada cantidad de segundos\n"); // OK
    printf("  kill <pid>       - Mata un proceso dado su ID.\n");                                           // OK
    printf("  nice <pid> <pri> - Cambia la prioridad de un proceso dado su ID y la nueva prioridad\n");     // OK
    printf("  block <pid>      - Switch entre ready y blocked de un proceso dado su ID.\n");                // OK
    printf("  cat              - Imprime el stdin tal como lo recibe.\n");                                  // OK
    printf("  wc               - Cuenta la cantidad de líneas del input\n");                                // OK
    printf("  filter           - Filtra las vocales del input.\n");                                         // OK
    printf("  mvar             - Implementa el problema de múltiples lectores\n");                          // TODO

    printf("\nTests disponibles:\n");
    printf("  test_mm <max-bytes>                     - Ejecuta stress test del manejador de memoria\n");                    // OK
    printf("  test_processes <max-processes>          - Crea, bloquea, desbloquea y mata procesos aleatoriamente.\n");       // OK
    printf("  test_priority <end-val-for-process>     - 3 procesos se ejecutan con misma prioridad y luego con distinta\n"); // OK
    printf("  test_synchro <processes> <inc-dec>      - Varios procesos modifican 1 variable usando semaforos\n");           // OK
    printf("  test_no_synchro <processes> <inc-dec>   - Varios procesos modifican una variable sin semaforos\n");            // OK

    printf("\nControles:\n");
    printf("  comando_1 <params> | comando_2 <params> - Concatenar comandos\n");             // OK (en fg)
    printf("  & comando                               - Ejecutar proceso en background \n"); // TODO
    printf("  Ctrl+D                                  - Enviar EOT por STDIN\n");            // OK
    printf("  Ctrl+C                                  - Matar proceso en foreground\n");     // TODO
    putchar(EOT);
    exit(0);
    return 0;
}

static void cmd_clear()
{
    clear_screen();
    shell_print_prompt();
}

static int cmd_mem()
{
    size_t total, used, free;
    getMemInfo(&total, &used, &free);
    printf("Estado de la memoria:\n");
    printf("TOTAL: %d   USADO: %d   LIBRE: %d\n", total, used, free);
    putchar(EOT);
    exit(0);
    return 0;
}

int cmd_loop(void *argv)
{
    if (argv == NULL)
        return -1;
    int *args = (int *)argv;
    int segs = args[0];
    if (segs <= 0)
    {
        printf("Uso: loop <segundos>\n");
        putchar(EOT);
        exit(0);
    }
    while (1)
    {
        printf("Hola! soy el proceso: %d. Este mensaje aparecera cada %d segundo", (int)getpid(), segs);
        if (segs > 1)
            printf("s");
        printf("\n");
        sleep(segs);
    }
    putchar(EOT);
    exit(0);
    return 0;
}

static int cmd_ps()
{
    proc_info_t proc_list[MAX_PROC];
    int count = get_proc_list(proc_list, MAX_PROC);
    for (int i = 0; i < count; i++)
    {
        proc_info_t *p = &proc_list[i];
        printf(
            "PID:%d | Father:%d | Pri:%d | Ready:%d | Wait:%d | Zombie:%d | Status:%d\n",
            p->pid,
            p->father_pid,
            p->priority,
            p->ready,
            p->waiting,
            p->is_zombie,
            p->status);
    }
    putchar(EOT);
    exit(0);
    return 0;
}

static void cmd_kill(void *argv)
{
    if (argv == NULL)
    {
        putchar(EOT);
        exit(-1);
    }
    int *args = (int *)argv;
    pid_t pid = args[0];
    int status;
    if (pid == getpid())
    {
        status = kill(pid);
    }
    else
    {
        status = -1;
    }

    printf("Kill a proceso: %d termino con estado: %d \n", pid, status);
    putchar(EOT);
    exit(status);
}

static void cmd_nice(void *argv)
{
    if (argv == NULL)
    {
        putchar(EOT);
        exit(-1);
    }
    int *args = (int *)argv;
    pid_t pid = args[0];
    process_priority_t prio = args[1];
    if (prio < PRIORITY_LOW || prio > PRIORITY_HIGH)
    {
        printf("Error, prioridades disponibles:\nLOW : %d\nNORMAL : %d\nHIGH: %d\n", PRIORITY_LOW, PRIORITY_NORMAL, PRIORITY_HIGH);
        putchar(EOT);
        exit(-1);
    }
    printf("cambiando la prioridad del proceso: %d a %d\n", pid, prio);
    int status = set_priority(pid, prio);
    putchar(EOT);
    exit(status);
}

static void cmd_block(void *argv)
{
    if (argv == NULL)
    {
        putchar(EOT);
        exit(-1);
    }
    int *args = (int *)argv;
    pid_t pid = args[0];

    if (block_proc(pid) == 0)
    {
        printf("bloqueando el proceso: %d", pid);
        putchar(EOT);
        exit(0);
    }
    printf("desbloqueando el proceso: %d", pid);
    putchar(EOT);
    exit(unblock_proc(pid));
}

static void cmd_cat()
{
    while (1)
    {
        unsigned char c = getchar();
        if (c == EOT)
        {
            printf("\n");
            break;
        }
        putchar(c);
    }
    putchar(EOT);
    exit(0);
}

static void cmd_wc()
{
    int count = 0;
    while (1)
    {
        unsigned char c = getchar();
        if (c == EOT)
        {
            break;
        }
        if (c == '\n')
        {
            count++;
        }
    }
    printf("Lineas: %d\n", count);
    putchar(EOT);
    exit(count);
}

static void cmd_filter()
{
    while (1)
    {
        unsigned char c = getchar();
        if (c == EOT)
        {
            break;
        }
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' ||
            c == '\n' || c == ' ')
        {
            putchar(c);
        }
    }
    putchar(EOT);
    exit(0);
}

static void mvar_proc(void *argv);

static void cmd_mvar()
{
    char *shared_mm = (char *)calloc(1000, sizeof(char));
    char *mutex_sem_name = "mvar_mutex";
    char *print_sem_name = "mvar_print";
    int print_sem_id;
    int proc_pipes[3];
    pid_t proc_pids[3];

    if (shared_mm == NULL)
    {
        printf("Error al reservar memoria compartida\n");
        putchar(EOT);
        exit(-1);
    }
    if ((print_sem_id = sem_open(print_sem_name, 0)) == -1)
    {
        printf("Error al crear semáforo en mvar\n");
        putchar(EOT);
        exit(-1);
    }

    char *argv[] = {shared_mm, mutex_sem_name, print_sem_name};
    for (int i = 0; i < 3; i++)
    {
        proc_pipes[i] = pipe_create();
        if (proc_pipes[i] == -1)
        {
            printf("Error al crear pipe para el proceso %d\n", i);
            putchar(EOT);
            exit(-1);
        }
        proc_pids[i] = new_proc((task_fn_t)mvar_proc, (void *)argv);
        fd_bind_std(proc_pids[i], 1, proc_pipes[i]);
    }

    int next = 0;
    while (1)
    {
        sem_wait(print_sem_id);
        unsigned char buffer[STD_BUFF_SIZE];
        for (int i = 1; i <= 3; i++)
        {
            if (!pipe_available(proc_pipes[next]))
            {
                next = (i + 1) % 3;
                continue;
            }
            else
            {
                int n = read(proc_pipes[next], buffer, STD_BUFF_SIZE - 1);
                if (n > 0)
                {
                    buffer[n] = '\0';
                    printf("%s", buffer);
                }
                next = (i + 1) % 3;
                break;
            }
        }
    }
    putchar(EOT);
    exit(0);
}

static void mvar_proc(void *argv)
{
    if (argv == NULL)
    {
        putchar(EOT);
        exit(-1);
    }
    char **args = (char **)argv;
    char *shared_mm = args[0];
    char *mutex_sem_name = args[1];
    char *print_sem_name = args[2];
    if (shared_mm == NULL || mutex_sem_name == NULL || print_sem_name == NULL)
    {
        putchar(EOT);
        exit(-1);
    }

    int mutex_sem_id = sem_open(mutex_sem_name, 1);
    int print_sem_id = sem_open(print_sem_name, 0);

    if (mutex_sem_id == -1 || print_sem_id == -1)
    {
        putchar(EOT);
        exit(-1);
    }

    while (1)
    {
        sem_wait(mutex_sem_id);
        printf("Proceso %d accediendo a la memoria compartida\n", (int)getpid());
        sleep(1); // simula tiempo de procesamiento para evitar la ejecución en un quantum.
        printf("Proceso %d leyendo del contenido de la memoria compartida: %s\n", (int)getpid(), shared_mm);
        sleep(1);
        printf("Proceso %d escribiendo en la memoria compartida\n", (int)getpid());

        strcpy(shared_mm, "El proceso: ");
        char *pid_str = itoa_malloc((int)getpid());
        if (pid_str != NULL)
        {
            strcpy(shared_mm + strlen(shared_mm), pid_str);
            free(pid_str);
        }
        else
        {
            strcpy(shared_mm + strlen(shared_mm), "unknown PID ");
        }
        strcpy(shared_mm + strlen(shared_mm), "Estuvo aqui.\n");
        sem_post(print_sem_id); // notificar cambios en stdout
        sem_post(mutex_sem_id);
    }
    putchar(EOT);
    exit(0);
}

static int cmd_testMM(void *argv)
{
    int *args = (int *)argv;
    const char *usage = "Uso: testMM <bytes>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        putchar(EOT);
        exit(-1);
    }
    printf("Iniciando testMM con %d bytes\n", args[0]);
    int result = test_mm(args[0]);
    printf("testMM finalizado con codigo %d\n", result);
    putchar(EOT);
    exit(result);
    return result;
}

static int cmd_testProcesses(void *argv)
{
    int *args = (int *)argv;
    const char *usage = "Uso: test_processes <max-processes>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        putchar(EOT);
        exit(-1);
    }

    printf("Iniciando testProcesses con max %d procesos...\n", args[0]);
    int result = test_processes(args[0]);
    printf("testProcesses finalizado con codigo %d\n", result);
    putchar(EOT);
    exit(result);
    return result;
}

static int cmd_testPriority(void *argv)
{
    int *args = (int *)argv;
    const char *usage = "Uso: test_priority <end-val-for-process>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        putchar(EOT);
        exit(-1);
    }
    printf("Iniciando testPriority con max_val %d ...\n", args[0]);
    int result = test_prio(args[0]);
    printf("testPriority finalizado con codigo %d\n", result);
    putchar(EOT);
    exit(result);
    return result;
}

static int cmd_testSynchro(void *argv)
{
    int *args = (int *)argv;

    const char *usage = "Uso: test_sync <max-val>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        putchar(EOT);
        exit(-1);
    }
    printf("Iniciando test CON semaforos     max-val: %d\n", args[0]);
    int result = test_sync(args[0], 1);
    printf("testSynchro finalizado con codigo %d\n", result);
    putchar(EOT);
    exit(result);
    return result;
}

static int cmd_testNoSynchro(void *argv)
{
    int *args = (int *)argv;

    const char *usage = "Uso: test_no_synchro <max-val>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        putchar(EOT);
        exit(-1);
    }
    printf("Iniciando test SIN semaforos     max-val: %d\n", args[0]);
    int result = test_sync(args[0], 0);
    printf("testNoSynchro finalizado con codigo %d\n", result);
    putchar(EOT);
    exit(result);
    return result;
}

// fin comandos diponibles

static void print_error(const char *msg);
pid_t launch_program(char *cmd, char **args);

void execute_tokenized_command(char **tokens, int token_count, int foreground_mode, int left_pipe_args, int right_pipe_args)
{
    pid_t left_pid = -1;
    pid_t right_pid = -1;

    // validaciones
    if (tokens == NULL || token_count <= 0 || left_pipe_args < 0)
    {
        print_error("Comando invalido");
        return;
    }

    if (strcmp("clear", tokens[left_pipe_args]) == 0)
    {
        cmd_clear();
        return;
    }

    // creación de procesos
    if (right_pipe_args >= 0)
    {
        left_pid = launch_program(tokens[left_pipe_args], &tokens[left_pipe_args + 1]);
        if (left_pid <= 1)
        {
            return;
        }
        block_proc(left_pid);
        right_pid = launch_program(tokens[right_pipe_args], &tokens[right_pipe_args + 1]);
        if (right_pid <= 1)
        {
            kill(left_pid);
            return;
        }
        block_proc(right_pid);
    }
    else
    {
        left_pid = launch_program(tokens[left_pipe_args], &tokens[left_pipe_args + 1]);
        if (left_pid <= 1)
        {
            return;
        }
        block_proc(left_pid);
    }

    // bindings de pipes entre procesos
    if (left_pid > 1 && right_pid > 1)
    {
        int proc_pipe = pipe_create();
        if (proc_pipe == -1)
        {
            print_error("No se pudo crear pipe entre los procesos");
            kill(left_pid);
            kill(right_pid);
            return;
        }
        fd_bind_std(left_pid, STDOUT, proc_pipe);
        fd_bind_std(right_pid, STDIN, proc_pipe);
    }

    int pipe_stdout = get_shell_stdout_pipe();
    if (pipe_stdout <= 0)
    {
        print_error("No se pudo crear pipe entre los procesos");
        kill(left_pid);
        kill(right_pid);
        return;
    }

    if (right_pid > 1)
    {
        fd_bind_std(right_pid, STDOUT, pipe_stdout);
    }
    else
    {
        fd_bind_std(left_pid, STDOUT, pipe_stdout);
    }

    if (foreground_mode)
    {
        fd_bind_std(left_pid, STDIN, STDIN);
        set_left_fg_proc(left_pid);
        right_pid > 1 ? set_right_fg_proc(right_pid) : set_right_fg_proc(-1);
    }
    unblock_proc(left_pid);
    unblock_proc(right_pid);
    /*


    if (foreground_mode)
    {
        if (right_pid > 1)
        {
            int shell_pipe = pipe_create();
            if (shell_pipe == -1)
            {
                print_error("No se pudo crear pipe entre shell y proceso");
                kill(left_pid);
                kill(right_pid);
                return;
            }
            fd_bind_std(left_pid, STDIN, STDIN);        // stdin de la shell
            fd_bind_std(right_pid, STDOUT, shell_pipe); // salida a shell
            fd_bind_std(getpid(), STDIN, shell_pipe);   // stdin de la shell
            unblock_proc(left_pid);
            unblock_proc(right_pid);

            set_left_fg_proc(left_pid);
            set_right_fg_proc(right_pid);
        }
        else
        {
            int shell_pipe = pipe_create();
            if (shell_pipe == -1)
            {
                print_error("No se pudo crear pipe entre shell y proceso");
                kill(left_pid);
                return;
            }
            fd_bind_std(left_pid, STDIN, STDIN);       // salida a shell
            fd_bind_std(left_pid, STDOUT, shell_pipe); // salida a shell
            fd_bind_std(getpid(), STDIN, shell_pipe);  // stdin de la shell
            unblock_proc(left_pid);
            set_left_fg_proc(left_pid);
            set_right_fg_proc(-1);
        }
    }
    else
    {
        if (right_pid > 1)
        {
            unblock_proc(right_pid);
        }
        unblock_proc(left_pid);
    }
    */
}

int argv[2];
pid_t launch_program(char *cmd, char **args)
{
    argv[0] = strtoint(args[0]);
    argv[1] = strtoint(args[1]);

    if (!strcmp(cmd, "help"))
    {
        return new_proc((task_fn_t)cmd_help, NULL);
    }
    else if (!strcmp(cmd, "mem"))
    {
        return new_proc((task_fn_t)cmd_mem, NULL);
    }
    else if (!strcmp(cmd, "ps"))
    {
        return new_proc((task_fn_t)cmd_ps, NULL);
    }
    else if (!strcmp(cmd, "loop"))
    {
        return new_proc((task_fn_t)cmd_loop, argv);
    }
    else if (!strcmp(cmd, "kill"))
    {
        return new_proc((task_fn_t)cmd_kill, argv);
    }
    else if (!strcmp(cmd, "nice"))
    {
        return new_proc((task_fn_t)cmd_nice, argv);
    }
    else if (!strcmp(cmd, "block"))
    {
        return new_proc((task_fn_t)cmd_block, argv);
    }
    else if (!strcmp(cmd, "cat"))
    {
        return new_proc((task_fn_t)cmd_cat, NULL);
    }
    else if (!strcmp(cmd, "wc"))
    {
        return new_proc((task_fn_t)cmd_wc, NULL);
    }
    else if (!strcmp(cmd, "filter"))
    {
        return new_proc((task_fn_t)cmd_filter, NULL);
    }
    else if (!strcmp(cmd, "mvar"))
    {
        return new_proc((task_fn_t)cmd_mvar, NULL);
    }
    else if (!strcmp(cmd, "test_mm"))
    {
        return new_proc((task_fn_t)cmd_testMM, argv);
    }
    else if (!strcmp(cmd, "test_processes"))
    {
        return new_proc((task_fn_t)cmd_testProcesses, argv);
    }
    else if (!strcmp(cmd, "test_priority"))
    {
        return new_proc((task_fn_t)cmd_testPriority, argv);
    }
    else if (!strcmp(cmd, "test_synchro"))
    {
        return new_proc((task_fn_t)cmd_testSynchro, argv);
    }
    else if (!strcmp(cmd, "test_no_synchro"))
    {
        return new_proc((task_fn_t)cmd_testNoSynchro, argv);
    }
    else
    {
        shell_print_colored("Error: ", ERROR_COLOR);
        shell_print_colored("Comando ", FONT_COLOR);
        shell_print_colored(cmd, FONT_COLOR);
        shell_print_colored(" desconodido.\n", FONT_COLOR);
        shell_print_colored("Escribe \"help\" para ver comandos disponibles.\n", FONT_COLOR);
    }
    return -1;
}

static void print_error(const char *msg)
{
    shell_print_colored("Error: ", ERROR_COLOR);
    shell_print_colored(msg, FONT_COLOR);
    shell_print_colored("\nEscribe \"help\" para ver comandos disponibles.\n", FONT_COLOR);
}

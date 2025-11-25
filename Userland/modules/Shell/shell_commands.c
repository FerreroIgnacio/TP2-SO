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

extern void rebuild_line_visual();
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
    printf("  mvar             - Implementa el problema de múltiples lectores\n");                          // OK

    printf("\nTests disponibles:\n");
    printf("  test_mm <max-bytes>                     - Ejecuta stress test del manejador de memoria\n");                    // OK
    printf("  test_processes <max-processes>          - Crea, bloquea, desbloquea y mata procesos aleatoriamente.\n");       // OK
    printf("  test_priority <end-val-for-process>     - 3 procesos se ejecutan con misma prioridad y luego con distinta\n"); // OK
    printf("  test_synchro <processes> <inc-dec>      - Varios procesos modifican 1 variable usando semaforos\n");           // OK
    printf("  test_no_synchro <processes> <inc-dec>   - Varios procesos modifican una variable sin semaforos\n");            // OK

    printf("\nControles:\n");
    printf("  comando_1 <params> | comando_2 <params> - Concatenar comandos\n");             // OK
    printf("  & comando                               - Ejecutar proceso en background \n"); // OK
    printf("  Ctrl+D                                  - Enviar EOT por STDIN\n");            // OK
    printf("  Ctrl+C                                  - Matar proceso en foreground\n");     // OK
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

static int cmd_loop(void *argv)
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
        sleep(segs * 1000);
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

    if (pid != getpid())
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
        printf("bloqueando el proceso: %d.", pid);
        putchar(EOT);
        exit(0);
    }
    printf("desbloqueando el proceso: %d.", pid);
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
            break;
        }
        putchar(c);
    }
    printf("\n");
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
    printf("Se recibieron %d lineas.\n", count);
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
    printf("\n");
    putchar(EOT);
    exit(0);
}

#define MVAR_MAX_PROC 6
void mvar_reader(void *argv)
{
    char **args = (char **)argv;
    char *name = args[0];
    char *shared = args[1];

    int write_sem_id = sem_open("mvar_write", 1);
    int read_sem_id = sem_open("mvar_read", 0);
    while (1)
    {
        sem_wait(read_sem_id);
        char readed = *shared;
        *shared = 0;
        sem_post(write_sem_id);
        printf("(%s%c) ", name, readed);
        yield();
    }
}

void mvar_writer(void *argv)
{
    char **args = (char **)argv;
    char *name = args[0];
    char *shared = args[1];

    int write_sem_id = sem_open("mvar_write", 1);
    int read_sem_id = sem_open("mvar_read", 0);
    while (1)
    {
        sem_wait(write_sem_id);
        *shared = *name;
        sem_post(read_sem_id);
        yield();
    }
}

char *mvar_argv[2];
char *readers_names[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
char *writers_names[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};

void cmd_mvar(void *argv)
{
    int *args = (int *)argv;
    int writers_count = args[0];
    int readers_count = args[1];
    int mvar_pipes[MVAR_MAX_PROC] = {0};
    int writer_to_kill;
    int reader_to_kill;

    if (writers_count < 1 || readers_count < 1 || writers_count > MVAR_MAX_PROC || readers_count > MVAR_MAX_PROC)
    {
        printf("Error: solo se permiten solamente entre 2 y %d escritores / lectores.\n", MVAR_MAX_PROC);
        putchar(EOT);
        exit(-1);
    }
    char *shared = malloc(1);
    if (shared == NULL)
    {
        printf("Error al reservar memoria compartida\n");
        putchar(EOT);
        exit(-1);
    }
    int write_sem_id = sem_open("mvar_write", 1);
    int read_sem_id = sem_open("mvar_read", 0);
    if (write_sem_id < 0 || read_sem_id < 0)
    {
        printf("Error al crear los semáforos\n");
        putchar(EOT);
        exit(-1);
    }

    printf("Iniciando mvar con %d escritores y %d lectores...\n", writers_count, readers_count);

    for (int i = 0; i < readers_count; i++)
    {
        mvar_pipes[i] = pipe_create();
        if (mvar_pipes[i] < 0)
            break;
        mvar_argv[0] = readers_names[i];
        mvar_argv[1] = shared;
        int pid = new_proc((task_fn_t)mvar_reader, mvar_argv);
        if (pid <= 1)
            break;
        fd_bind_std(pid, STDOUT, mvar_pipes[i]);
        if (i == 0)
            reader_to_kill = pid;

        yield();
    }

    for (int i = 0; i < writers_count; i++)
    {
        mvar_argv[0] = writers_names[i];
        mvar_argv[1] = shared;
        int pid = new_proc((task_fn_t)mvar_writer, mvar_argv);
        if (pid <= 1)
            break;
        if (i == 0)
            writer_to_kill = pid;
        yield();
    }

    printf("Escritores: A , B ...\n");
    printf("Lectores: 1 , 2 ...\n");
    printf("Cuando se lee, se elimina el valor en la memoria compartida. \n");
    printf("Presione 'w' para matar a un escritor y 'r' para matar a un lector. \n");

    while (1)
    {
        if (writer_to_kill || reader_to_kill)
        {
            if (pipe_available(STDIN))
            {
                char c = getchar();
                if ((c == 'w' || c == 'W') && writer_to_kill)
                {
                    kill(writer_to_kill);
                    writer_to_kill = 0;
                    printf("\nEscritor A eliminado.\n");
                }
                if ((c == 'r' || c == 'R') && reader_to_kill)
                {
                    kill(reader_to_kill);
                    reader_to_kill = 0;
                    printf("\nLector 0 eliminado.\n");
                }
            }
        }

        for (int i = 0; i < readers_count; i++)
        {
            if (pipe_available(mvar_pipes[i]) > 0)
            {
                unsigned char buffer[STD_BUFF_SIZE];
                int bytes_read = pipe_read(mvar_pipes[i], (char *)buffer, STD_BUFF_SIZE - 1);
                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';
                    printf("%s", buffer);
                }
            }
        }

        yield();
    }
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
static pid_t launch_program(char *cmd, char **args, int side);

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
        left_pid = launch_program(tokens[0], &tokens[1], 1);
        if (left_pid <= 1)
        {
            return;
        }
        block_proc(left_pid);
        right_pid = launch_program(tokens[right_pipe_args], &tokens[right_pipe_args + 1], 0);
        if (right_pid <= 1)
        {
            kill(left_pid);
            return;
        }
        block_proc(right_pid);
    }
    else
    {
        left_pid = launch_program(tokens[0], &tokens[1], 1);
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
}

// fix temporal
int argv_l[2];
int argv_r[2];

static pid_t launch_program(char *cmd, char **args, int side)
{
    // fix temporal
    int *argv;
    if (side)
    {
        argv_l[0] = strtoint(args[0]);
        argv_l[1] = strtoint(args[1]);
        argv = argv_l;
    }
    else
    {
        argv_r[0] = strtoint(args[0]);
        argv_r[1] = strtoint(args[1]);
        argv = argv_r;
    }
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
        return new_proc((task_fn_t)cmd_mvar, argv);
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
        shell_print_prompt();
        rebuild_line_visual();
    }
    return -1;
}

static void print_error(const char *msg)
{
    shell_print_colored("Error: ", ERROR_COLOR);
    shell_print_colored(msg, FONT_COLOR);
    shell_print_colored("\nEscribe \"help\" para ver comandos disponibles.\n", FONT_COLOR);
}

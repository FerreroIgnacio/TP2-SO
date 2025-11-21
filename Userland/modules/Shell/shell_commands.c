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

// Forward declarations of shell print helpers to avoid implicit declaration warnings
static void sp_print(const char *s);
static void sp_uint(uint64_t v);
static void sp_dec(int64_t v);
static void sp_hex(uint64_t v);
static void shell_printf(const char *fmt, ...); // nueva función de formato
// Synchronous execution helper: crea proceso y espera a que termine
static void run_sync(task_fn_t fn, void *arg, int free_after)
{
    pid_t pid = new_proc(fn, arg);
    if (pid >= 0)
    {
        int st;
        waitpid(pid, &st, WHANG);
    }
    if (free_after && arg)
    {
        free(arg);
    }
}

#define MAX_PROCESS 100

static void *const pongisgolfModuleAddress = (void *)0x8000000;
static char *dup_args(const char *args);
static int run_test_sync_cmd(void *argv, int use_sem, const char *usage);

static char *dup_args(const char *args)
{
    if (args == NULL)
    {
        return NULL;
    }
    size_t len = strlen(args) + 1;
    char *copy = malloc(len);
    if (copy)
    {
        strcpy(copy, args);
    }
    return copy;
}

static void shell_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':
            {
                int v = va_arg(ap, int);
                sp_dec(v);
                break;
            }
            case 'u':
            {
                unsigned int v = va_arg(ap, unsigned int);
                sp_uint(v);
                break;
            }
            case 'x':
            case 'X':
            {
                unsigned int v = va_arg(ap, unsigned int);
                sp_hex(v);
                break;
            }
            case 's':
            {
                const char *s = va_arg(ap, const char *);
                sp_print(s);
                break;
            }
            case 'c':
            {
                int c = va_arg(ap, int);
                shell_putchar((char)c);
                break;
            }
            case '%':
            {
                shell_putchar('%');
                break;
            }
            default:
                shell_putchar('%');
                shell_putchar(*fmt);
                break;
            }
        }
        else
        {
            shell_putchar(*fmt);
        }
        if (*fmt)
            fmt++;
    }
    va_end(ap);
}

static int run_test_sync_cmd(void *argv, int use_sem, const char *usage)
{
    char *args = (char *)argv;
    if (!args)
    {
        shell_printf("%s\n", usage);
        goto cleanup;
    }
    while (*args == ' ')
        args++;
    if (*args == '\0')
    {
        shell_printf("%s\n", usage);
        goto cleanup;
    }
    char *arg_end = args;
    while (*arg_end && *arg_end != ' ')
        arg_end++;
    if (*arg_end != '\0')
    {
        char *extra = arg_end + 1;
        while (*extra == ' ')
            extra++;
        if (*extra != '\0')
        {
            shell_printf("%s\n", usage);
            goto cleanup;
        }
    }
    *arg_end = '\0';
    char *test_args[] = {args, use_sem ? "1" : "0"};
    shell_printf("Iniciando test_sync %s con n=%s\n", use_sem ? "con semaforos" : "sin semaforos", args);
    uint64_t result = test_sync(2, test_args);
    shell_printf("test_sync finalizado con codigo %x\n", (unsigned int)result);
    if (argv)
        free(argv);
    exit((int)result);
cleanup:
    if (argv)
        free(argv);
    exit(1);
    return 1;
}

// Comandos disponibles

// CMD_HELP DE ARQUITECTURA DE COMPUTADORAS
/*
void cmd_help()
{
    printf("Comandos disponibles:\n");
    printf("  help             - Mostrar comandos disponibles\n");
    printf("  clear            - Limpiar pantalla\n");
    printf("  echo <message>    - Mostrar texto en pantalla\n");
    printf("  datetime         - Mostrar fecha y hora UTC-0 \n");
    printf("  registers        - Imprimir registros guardados (F1)\n");
    printf("  listfonts        - Listar las fuentes disponibles\n");
    printf("  setfont <id>     - Cambiar la fuente\n");
    printf("  testzerodiv      - Testea la excepcion 00 \n");
    printf("  testinvalidcode  - Testea la excepcion 06 \n");
    printf("\nProgramas disponibles:\n");
    printf("  pongisgolf\n");
    printf("\nControles:\n");
    printf("  Enter - Ejecutar comando\n");
    printf("  Backspace - Borrar caracter\n");
}*/

int cmd_help()
{
    sp_print("Comandos disponibles:\n");
    sp_print("  help             - Mostrar comandos disponibles\n");                                          // OK
    sp_print("  clear            - Limpiar pantalla\n");                                                      // OK
    sp_print("  mem              - Imprime el estado de la memoria\n");                                       // OK
    sp_print("  ps               - Imprime la lista de todos los procesos\n");                                // OK
    sp_print("  loop <segundos>  - Imprime su ID con un saludo cada una determinada cantidad de segundos\n"); // OK
    sp_print("  kill <pid>       - Mata un proceso dado su ID.\n");                                           // OK
    sp_print("  nice <pid> <pri> - Cambia la prioridad de un proceso dado su ID y la nueva prioridad\n");     // OK
    sp_print("  block <pid>      - Switch entre ready y blocked de un proceso dado su ID.\n");                // OK
    sp_print("  cat              - Imprime el stdin tal como lo recibe.\n");                                  // TODO
    sp_print("  wc               - Cuenta la cantidad de líneas del input\n");                                // TODO
    sp_print("  filter           - Filtra las vocales del input.\n");                                         // TODO
    sp_print("  mvar             - Implementa el problema de múltiples lectores\n");                          // TODO
    // Nuevos comandos de FDs dinamicos (por proceso)
    sp_print("  createfd <name>  - Crea un FD dinamico en este proceso (desde 3 en adelante)\n");
    sp_print("  writefd <fd> <texto> - Escribe texto en un FD dinamico de este proceso\n");
    sp_print("  readfd <fd>      - Lee y muestra el contenido de un FD dinamico de este proceso\n");
    sp_print("  fdlist           - Lista los FDs dinamicos del proceso actual\n");

    sp_print("\nTests disponibles:\n");
    sp_print("  test_mm <max-bytes>                     - Ejecuta stress test del manejador de memoria\n");                    // OK
    sp_print("  test_processes <max-processes>          - Crea, bloquea, desbloquea y mata procesos aleatoriamente.\n");       // OK
    sp_print("  test_priority <end-val-for-process>     - 3 procesos se ejecutan con misma prioridad y luego con distinta\n"); // OK
    sp_print("  test_synchro <processes> <inc-dec>      - Varios procesos modifican 1 variable usando semaforos\n");           // TODO
    sp_print("  test_no_synchro <processes> <inc-dec>   - Varios procesos modifican una variable sin semaforos\n");            // TODO

    sp_print("\nControles:\n");
    sp_print("  Enter - Ejecutar comando\n");
    sp_print("  Backspace - Borrar caracter\n");

    exit(0);
    return 0;
}

void cmd_clear()
{
    clear_screen();
}

int cmd_mem()
{
    size_t total, used, freeMem;
    getMemInfo(&total, &used, &freeMem);
    sp_print("Estado de la memoria:\nTOTAL: ");
    sp_dec((int)total);
    sp_print("   USADO: ");
    sp_dec((int)used);
    sp_print("   LIBRE: ");
    sp_dec((int)freeMem);
    sp_print("\n");
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
        exit(0);
    }
    while (1)
    {
        printf("Hola! soy el proceso: %d. Este mensaje aparecera cada %d segundos \n", (int)getpid(), segs);
        sleep(segs);
    }
    exit(0);
    return 0;
}

int cmd_ps()
{
    proc_info_t proc_list[MAX_PROCESS];
    int count = get_proc_list(proc_list, MAX_PROCESS);
    for (int i = 0; i < count; i++)
    {
        proc_info_t *p = &proc_list[i];
        sp_print("PID:");
        sp_dec(p->pid);
        sp_print(" | Father:");
        sp_dec(p->father_pid);
        sp_print(" | Pri:");
        sp_dec(p->priority);
        sp_print(" | Ready:");
        sp_dec(p->ready);
        sp_print(" | Wait:");
        sp_dec(p->waiting);
        sp_print(" | Zombie:");
        sp_dec(p->is_zombie);
        sp_print(" | Status:");
        sp_dec(p->status);
        sp_print("\n");
    }
    exit(0);
    return 0;
}

void cmd_kill(void *argv)
{
    if (argv == NULL)
    {
        exit(-1);
    }
    int *args = (int *)argv;
    pid_t pid = args[0];
    int status = kill(pid);
    shell_printf("Kill a proceso: %d termino con estado: %d \n", pid, status);
    exit(status);
}

void cmd_nice(void *argv)
{
    if (argv == NULL)
    {
        exit(-1);
    }
    int *args = (int *)argv;
    pid_t pid = args[0];
    process_priority_t prio = args[1];
    if (prio < PRIORITY_LOW || prio > PRIORITY_HIGH)
    {
        shell_printf("Error, prioridades disponibles:\nLOW : %d\nNORMAL : %d\nHIGH: %d\n", PRIORITY_LOW, PRIORITY_NORMAL, PRIORITY_HIGH);
        exit(-1);
    }
    shell_printf("cambiando la prioridad del proceso: %d a %d\n", pid, prio);
    int status = set_priority(pid, prio);
    exit(status);
}

void cmd_block(void *argv)
{
    if (argv == NULL)
    {
        exit(-1);
    }
    int *args = (int *)argv;
    pid_t pid = args[0];

    if (block_proc(pid) == 0)
    {
        printf("bloqueando el proceso: %d", pid);
        exit(0);
    }
    printf("desbloqueando el proceso: %d", pid);
    exit(unblock_proc(pid));
}

int cmd_testMM(void *argv)
{
    int *args = (int *)argv;
    const char *usage = "Uso: testMM <bytes>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        exit(-1);
    }
    printf("Iniciando testMM con %d bytes\n", args[0]);
    int result = test_mm(args[0]);
    printf("testMM finalizado con codigo %d\n", result);
    exit(result);
    return result;
}

int cmd_testProcesses(void *argv)
{
    int *args = (int *)argv;
    const char *usage = "Uso: test_processes <max-processes>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        exit(-1);
    }

    printf("Iniciando testProcesses con max %d procesos...\n", args[0]);
    int result = test_processes(args[0]);
    printf("testProcesses finalizado con codigo %d\n", result);
    exit(result);
    return result;
}

int cmd_testPriority(void *argv)
{
    int *args = (int *)argv;
    const char *usage = "Uso: test_priority <end-val-for-process>";
    if (argv == NULL || args[0] <= 1)
    {
        printf("%s\n", usage);
        exit(-1);
    }
    printf("Iniciando testPriority con max_val %d ...\n", args[0]);
    int result = test_prio(args[0]);
    printf("testPriority finalizado con codigo %d\n", result);
    exit(result);
    return result;
}

int cmd_testSynchro(void *argv) // TODO
{
    return run_test_sync_cmd(argv, 1, "Uso: test_synchro <n>");
}
int cmd_testNoSynchro(void *argv) // TODO
{
    return run_test_sync_cmd(argv, 0, "Uso: test_no_synchro <n>");
}

// ARQUI

// Proceso echo para uso en pipeline: escribe argv a STDOUT (pipe) y retorna
int echo_proc(void *argv)
{
    char *arg = (char *)argv;
    if (arg && *arg)
    {
        write(STDOUT, arg, strlen(arg));
    }
    write(STDOUT, "\n", 1);
    if (arg)
        free(arg);
    return 0;
}

void cmd_echo(char *args)
{
    if (args && *args)
    {
        // Standalone builtin: imprimir directo al framebuffer
        shell_print(args);
        shell_print("\n");
    }
    else
    {
        shell_newline();
    }
}

void cmd_dateTime()
{
    uint8_t year = 0, month = 0, day = 0, h = 0, m = 0, s = 0;
    getLocalTime(&h, &m, &s);
    getLocalDate(&year, &month, &day);
    sp_print("Fecha y hora en UTC-0: ");
    sp_dec(day);
    sp_print("/");
    sp_dec(month);
    sp_print("/20");
    sp_dec(year);
    sp_print(" ");
    sp_dec(h);
    sp_print(":");
    sp_dec(m);
    sp_print(":");
    sp_dec(s);
    sp_print("\n");
}

void cmd_registers()
{
    registers_t regs;
    getRegisters(&regs);
    sp_print("RIP:    0x");
    sp_hex(regs.rip);
    sp_print("\nRFLAGS: 0x");
    sp_hex(regs.rflags);
    sp_print("\nRSP:    0x");
    sp_hex(regs.rsp);
    sp_print("\nRBP:    0x");
    sp_hex(regs.rbp);
    sp_print("\nRAX:    0x");
    sp_hex(regs.rax);
    sp_print("\nRBX:    0x");
    sp_hex(regs.rbx);
    sp_print("\nRCX:    0x");
    sp_hex(regs.rcx);
    sp_print("\nRDX:    0x");
    sp_hex(regs.rdx);
    sp_print("\nRSI:    0x");
    sp_hex(regs.rsi);
    sp_print("\nRDI:    0x");
    sp_hex(regs.rdi);
    sp_print("\nR8 :    0x");
    sp_hex(regs.r8);
    sp_print("\nR9 :    0x");
    sp_hex(regs.r9);
    sp_print("\nR10:    0x");
    sp_hex(regs.r10);
    sp_print("\nR11:    0x");
    sp_hex(regs.r11);
    sp_print("\nR12:    0x");
    sp_hex(regs.r12);
    sp_print("\nR13:    0x");
    sp_hex(regs.r13);
    sp_print("\nR14:    0x");
    sp_hex(regs.r14);
    sp_print("\nR15:    0x");
    sp_hex(regs.r15);
    sp_print("\n");
}

void cmd_test0Div()
{
    int x = 1;
    int y = 0;
    int z = x / y;
    z++;
}

void cmd_createfd(char *args)
{
    if (!args || *args == '\0')
    {
        sp_print("Uso: createfd <name>\n");
        return;
    }
    int fd = fd_open(args);
    if (fd < 0)
    {
        sp_print("Error: no se pudo crear FD '");
        sp_print(args);
        sp_print("'\n");
        return;
    }
    sp_print("FD creado: ");
    sp_dec(fd);
    sp_print("\n");
}
void cmd_writefd(char *args)
{
    if (!args || *args == '\0')
    {
        sp_print("Uso: writefd <fd> <texto>\n");
        return;
    }
    char *fd_str = args;
    char *msg = find_args(args);
    if (!msg || *msg == '\0')
    {
        sp_print("Uso: writefd <fd> <texto>\n");
        return;
    }
    int fd = strtoint(fd_str);
    int wrote = write(fd, msg, strlen(msg));
    if (wrote < 0)
    {
        sp_print("Error: write a fd ");
        sp_dec(fd);
        sp_print("\n");
        return;
    }
    sp_print("Escritos ");
    sp_dec(wrote);
    sp_print(" bytes en fd ");
    sp_dec(fd);
    sp_print("\n");
}
void cmd_readfd(char *args)
{
    if (!args || *args == '\0')
    {
        sp_print("Uso: readfd <fd>\n");
        return;
    }
    int fd = strtoint(args);
    unsigned char buf[256];
    int total = 0;
    int chunk;
    // Leer al menos una vez; continuar sólo si buffer se llenó completamente (indica posible más datos disponibles inmediatamente)
    while (1)
    {
        chunk = read(fd, buf, sizeof(buf) - 1);
        if (chunk < 0)
        {
            sp_print("Error de lectura\n");
            break;
        }
        if (chunk == 0)
        {
            break;
        }
        buf[chunk] = '\0';
        shell_print((char *)buf);
        total += chunk;
        if (chunk < (int)(sizeof(buf) - 1))
        { // lectura parcial, no volver a bloquear esperando más
            break;
        }
    }
    if (total == 0)
    {
        sp_print("(sin datos)\n");
    }
    else
    {
        shell_printf("\nLeidos %d bytes de fd %d\n", total, fd);
    }
}

void shell_set_font(font_type_t font_index)
{
    int count = fontmanager_get_font_count();
    if (font_index < 0 || font_index >= count)
    {
        shell_print_colored("Error: indice de fuente inválido\n", ERROR_COLOR);
        return;
    }
    cmd_clear();
    fontmanager_set_font(font_index);
    shell_print_colored("Fuente cambiada a: ", PROMPT_COLOR);
    sp_print(fontmanager_get_font_name(font_index));
    sp_print("\n");
    shell_newline();
}
void shell_list_fonts()
{
    int count = fontmanager_get_font_count();
    sp_print("Fuentes disponibles:\n");
    for (int i = 0; i < count; i++)
    {
        const char *name = fontmanager_get_font_name(i);
        sp_print("  Id: ");
        sp_dec(i);
        sp_print("  -  ");
        sp_print(name);
        sp_print("\n");
    }
}
void cmd_fdlist()
{
    fd_info_u_t infos[32];
    int n = fd_list(infos, 32);
    if (n <= 0)
    {
        sp_print("(sin FDs dinamicos en este proceso)\n");
        return;
    }
    sp_print("FDs dinamicos del proceso actual (");
    sp_dec(n);
    sp_print("):\n");
    for (int i = 0; i < n; i++)
    {
        sp_print("  id=");
        sp_dec(infos[i].fd);
        sp_print(" name=");
        sp_print(infos[i].name);
        sp_print(" bytes=");
        sp_dec(infos[i].size);
        sp_print("\n");
    }
}

static int launch_cat(char *args);

int shell_launch_program(const char *name, char *args)
{
    if (name == NULL || *name == '\0')
        return -1;
    if (!strcmp(name, "cat"))
    {
        return launch_cat(args);
    }
    // otros programas externos en el futuro
    return -1;
}

extern int cat_proc(void *argv);

static int launch_cat(char *args)
{
    (void)args;
    return new_proc(cat_proc, NULL);
}

void command_switch(char *cmd_copy, char *args)
{
    if (!strcmp(cmd_copy, "help"))
    {
        run_sync((task_fn_t)cmd_help, NULL, 0);
    }
    else if (!strcmp(cmd_copy, "clear"))
    {
        cmd_clear();
    }
    else if (!strcmp(cmd_copy, "mem"))
    {
        run_sync(cmd_mem, NULL, 0);
    }
    else if (!strcmp(cmd_copy, "ps"))
    {
        run_sync(cmd_ps, NULL, 0);
    }
    else if (!strcmp(cmd_copy, "loop"))
    {
        int argv[] = {strtoint(args)};
        new_proc((task_fn_t)cmd_loop, argv);
    }
    else if (!strcmp(cmd_copy, "kill"))
    {
        int argv[] = {strtoint(args)};
        new_proc((task_fn_t)cmd_kill, argv);
    }
    else if (!strcmp(cmd_copy, "nice"))
    {
        char *p1 = strtok(args, " ");
        char *p2 = strtok(NULL, " ");
        int argv[] = {strtoint(p1), strtoint(p2)};
        new_proc((task_fn_t)cmd_nice, argv);
    }
    else if (!strcmp(cmd_copy, "block"))
    {
        int argv[] = {strtoint(args)};
        new_proc((task_fn_t)cmd_block, argv);
    }
    /*
    else if (!strcmp(cmd_copy, "cat"))
    {
    }
    else if (!strcmp(cmd_copy, "wc"))
    {
    }
    else if (!strcmp(cmd_copy, "filter"))
    {
    }
    */
    else if (!strcmp(cmd_copy, "test_mm"))
    {
        int argv[] = {strtoint(args)};
        new_proc((task_fn_t)cmd_testMM, argv);
    }
    else if (!strcmp(cmd_copy, "test_processes"))
    {
        int argv[] = {strtoint(args)};
        new_proc((task_fn_t)cmd_testProcesses, argv);
    }
    else if (!strcmp(cmd_copy, "test_priority"))
    {
        int argv[] = {strtoint(args)};
        new_proc((task_fn_t)cmd_testPriority, argv);
    }
    else if (!strcmp(cmd_copy, "test_synchro"))
    {
        int argv[] = {strtoint(args), 1};
        new_proc((task_fn_t)cmd_testSynchro, argv);
    }
    else if (!strcmp(cmd_copy, "test_no_synchro"))
    {
        int argv[] = {strtoint(args), 0};
        new_proc((task_fn_t)cmd_testSynchro, argv);
    }

    // Comandos realizados en Arquitectura de Computadoras (ya no se muestran en "help"):
    else if (!strcmp(cmd_copy, "echo"))
    {
        cmd_echo(args);
    }
    else if (!strcmp(cmd_copy, "datetime"))
    {
        cmd_dateTime();
    }
    else if (!strcmp(cmd_copy, "registers"))
    {
        cmd_registers();
    }
    else if (!strcmp(cmd_copy, "testzerodiv"))
    {
        cmd_test0Div();
    }
    else if (!strcmp(cmd_copy, "testinvalidcode"))
    {
        testInvalidCode();
    }
    else if (!strcmp(cmd_copy, "listfonts"))
    {
        shell_list_fonts();
    }
    else if (!strcmp(cmd_copy, "setfont"))
    {
        if (*args)
        {
            int font_index = strtoint(args);
            shell_set_font(font_index);
        }
        else
        {
            shell_print_colored("Uso: setfont <indice>\n", ERROR_COLOR);
        }
    }
    else if (!strcmp(cmd_copy, "createfd"))
    {
        cmd_createfd(args);
    }
    else if (!strcmp(cmd_copy, "writefd"))
    {
        cmd_writefd(args);
    }
    else if (!strcmp(cmd_copy, "readfd"))
    {
        cmd_readfd(args);
    }
    else if (!strcmp(cmd_copy, "fdlist"))
    {
        cmd_fdlist();
    }
    else if (!strcmp(cmd_copy, "pongisgolf"))
    {
        int current_font = fontmanager_get_current_font_index();
        EntryPoint run = (EntryPoint)pongisgolfModuleAddress;
        // Ejecuta módulo independiente (retorna al salir del juego)
        if (run)
        {
            run();
        }
        fontmanager_set_font(current_font);
        cmd_clear();
        shell_print_prompt();
    }
    else if (cmd_copy[0] != '\0')
    {
        // Intentar programa externo (no builtin)
        int pid = shell_launch_program(cmd_copy, args);
        if (pid < 0)
        {
            shell_print_colored("Error: ", ERROR_COLOR);
            shell_print("Comando desconocido '");
            shell_print(cmd_copy);
            shell_print("'\n");
            shell_print("Escribe 'help' para ver comandos disponibles.\n");
        }
    }
}

// Helper print functions for shell (framebuffer based)
static void sp_print(const char *s)
{
    if (s)
        shell_print(s);
}
static void sp_uint(uint64_t v)
{
    char buf[32];
    int i = 0;
    if (v == 0)
    {
        shell_putchar('0');
        return;
    }
    while (v)
    {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i--)
        shell_putchar(buf[i]);
}
static void sp_dec(int64_t v)
{
    if (v < 0)
    {
        shell_putchar('-');
        v = -v;
    }
    sp_uint((uint64_t)v);
}
static void sp_hex(uint64_t v)
{
    char buf[32];
    int i = 0;
    if (v == 0)
    {
        shell_putchar('0');
        return;
    }
    const char *d = "0123456789abcdef";
    while (v)
    {
        buf[i++] = d[v & 0xF];
        v >>= 4;
    }
    while (i--)
        shell_putchar(buf[i]);
}

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

#define MAX_PROCESS 100

static void *const pongisgolfModuleAddress = (void *)0x8000000;

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
    printf("Comandos disponibles:\n");
    printf("  help             - Mostrar comandos disponibles\n");                                          // OK
    printf("  clear            - Limpiar pantalla\n");                                                      // OK
    printf("  mem              - Imprime el estado de la memoria\n");                                       // OK
    printf("  ps               - Imprime la lista de todos los procesos\n");                                // OK
    printf("  loop <segundos>  - Imprime su ID con un saludo cada una determinada cantidad de segundos\n"); // OK: (falta fix en new_proc)
    printf("  kill <pid>       - Mata un proceso dado su ID.\n");                                           // OK: (falta hacerlo proceso)
    printf("  nice <pid> <pri> - Cambia la prioridad de un proceso dado su ID y la nueva prioridad\n");     // TODO
    printf("  block <pid>      - Switch entre ready y blocked de un proceso dado su ID.\n");                // OK
    printf("  cat              - Imprime el stdin tal como lo recibe.\n");                                  // TODO
    printf("  wc               - Cuenta la cantidad de líneas del input\n");                                // TODO
    printf("  filter           - Filtra las vocales del input.\n");                                         // TODO
    printf("  mvar             - Implementa el problema de múltiples lectores\n");                          // TODO

    printf("\nTests disponibles:\n");
    printf("  test_mm <max-bytes>                     - Ejecuta stress test del manejador de memoria\n");
    printf("  test_processes <max-processes>          - Crea, bloquea, desbloquea y mata procesos aleatoriamente.\n");       // OK: (falta fix en mm's + fix en new_proc)
    printf("  test_priority <end-val-for-process>     - 3 procesos se ejecutan con misma prioridad y luego con distinta\n"); // TODO
    printf("  test_synchro <processes> <inc-dec>      - Varios procesos modifican 1 variable usando semaforos\n");           // TODO
    printf("  test_no_synchro <processes> <inc-dec>   - Varios procesos modifican una variable sin semaforos\n");            // TODO

    printf("\nControles:\n");
    printf("  Enter - Ejecutar comando\n");
    printf("  Backspace - Borrar caracter\n");

    exit(0);
    return 0;
}

void cmd_clear()
{
    clear_screen();
}

int cmd_mem()
{
    size_t total, used, free;
    getMemInfo(&total, &used, &free);
    printf("Estado de la memoria:\n");
    printf("TOTAL: %d   USADO: %d   LIBRE: %d\n", total, used, free);
    exit(0);
    return 0;
}

int cmd_loop(void *argv) // TODO: FIX NEW_PROC
{
    printf("%s\n", argv);
    int segs = 5; // (int)strtoint(argv);
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
    exit(0);
    return 0;
}

void cmd_kill(pid_t pid)
{
    int status = kill(pid);
    printf("Kill a proceso: %d termino con estado: %d \n", pid, status);
}

void cmd_nice(pid_t pid, process_priority_t prio)
{
    printf("cambiando la prioridad del proceso: %d a %d", pid, prio);
    set_priority(pid, prio);
}

void cmd_block(pid_t pid)
{
    if (block_proc(pid) == 0)
    {
        printf("bloqueando el proceso: %d", pid);
        return;
    }
    printf("desbloqueando el proceso: %d", pid);
    unblock_proc(pid);
}

int cmd_testMM(void *argv)
{
    char *test[] = {"100000000"};
    test_mm(1, test);
    exit(0);

    char *args = (char *)argv;

    if (!args)
    {
        printf("Uso: testMM <bytes>\n");
        return 1;
    }

    while (*args == ' ')
    {
        args++;
    }

    if (*args == '\0')
    {
        printf("Uso: testMM <bytes>\n");
        return 1;
    }

    char *arg_end = args;
    while (*arg_end && *arg_end != ' ')
    {
        arg_end++;
    }

    char saved = *arg_end;
    *arg_end = '\0';

    if (*args == '\0')
    {
        printf("Uso: testMM <bytes>\n");
        *arg_end = saved;
        return 1;
    }

    if (saved != '\0')
    {
        char *extra = arg_end + 1;
        while (*extra == ' ')
        {
            extra++;
        }
        if (*extra != '\0')
        {
            printf("Uso: testMM <bytes>\n");
            *arg_end = saved;
            return 1;
        }
    }

    printf("Iniciando testMM con %s bytes\n", args);
    char *test_args[] = {args};
    uint64_t result = test_mm(1, test_args);
    printf("testMM finalizado con codigo %x\n", result);

    *arg_end = saved;
    exit(1);
    return 1;
}

int cmd_testProcesses(void *argv)
{
    char *args = (char *)argv;
    const char *usage_msg = "Uso: test_processes <max-processes>";

    if (!args)
    {
        printf("%s\n", usage_msg);
        return 1;
    }

    while (*args == ' ')
    {
        args++;
    }

    if (*args == '\0')
    {
        printf("%s\n", usage_msg);
        return 1;
    }

    char *arg_end = args;
    while (*arg_end && *arg_end != ' ')
    {
        arg_end++;
    }

    char saved = *arg_end;
    *arg_end = '\0';

    if (*args == '\0')
    {
        printf("%s\n", usage_msg);
        *arg_end = saved;
        return 1;
    }

    if (saved != '\0')
    {
        char *extra = arg_end + 1;
        while (*extra == ' ')
        {
            extra++;
        }
        if (*extra != '\0')
        {
            printf("%s\n", usage_msg);
            *arg_end = saved;
            return 1;
        }
    }

    printf("Iniciando testProcesses con max %s procesos...\n", args);
    char *test_args[] = {args};
    int64_t result = test_processes(1, test_args);
    printf("testProcesses finalizado con codigo %x\n", (uint32_t)result);

    *arg_end = saved;
    exit((int)result);
    return (int)result;
}

int cmd_testPriority(void *argv) // TODO
{
    printf("Iniciando testPriority...\n");

    char *test_args[] = {"3"}; //{argv};

    uint64_t result = test_prio(1, test_args);
    printf("testPriority finalizado con codigo %x\n", result);
    return result;
}

int cmd_testSynchro(void *argv) // TODO
{
    printf("Iniciando testSynchro...\n");
    // char *test_args[] = {args};
    uint64_t result = 0; // test_synchronization(0, test_args);
    printf("testSynchro finalizado con codigo %x\n", result);
    return result;
}

int cmd_testNoSynchro(void *argv) // TODO
{
    printf("Iniciando testNoSynchro...\n");
    // char *test_args[] = {args};
    uint64_t result = 0; // test_no_synchronization(0, test_args);
    printf("testNoSynchro finalizado con codigo %x\n", result);
    return result;
}

// ARQUI

void cmd_echo(char *args)
{
    if (*args)
    {
        printf("%s\n", args);
    }
    else
    {
        shell_newline();
    }
}

void cmd_dateTime()
{
    uint8_t year = 0, month = 0, day = 0, hours = 0, minutes = 0, seconds = 0;
    getLocalTime(&hours, &minutes, &seconds);
    getLocalDate(&year, &month, &day);
    printf("Fecha y hora en UTC-0: %d/%d/20%d %d:%d:%d\n", day, month, year, hours, minutes, seconds);
}

void cmd_registers()
{
    registers_t regs;
    getRegisters(&regs);
    printf("RIP:    %#P\n"
           "RFLAGS: %#P\n"
           "RSP:    %#P\n"
           "RBP:    %#P\n"
           "RAX:    %#P\n"
           "RBX:    %#P\n"
           "RCX:    %#P\n"
           "RDX:    %#P\n"
           "RSI:    %#P\n"
           "RDI:    %#P\n"
           "R8 :    %#P\n"
           "R9 :    %#P\n"
           "R10:    %#P\n"
           "R11:    %#P\n"
           "R12:    %#P\n"
           "R13:    %#P\n"
           "R14:    %#P\n"
           "R15:    %#P\n",
           regs.rip,
           regs.rflags,
           regs.rsp,
           regs.rbp,
           regs.rax,
           regs.rbx,
           regs.rcx,
           regs.rdx,
           regs.rsi,
           regs.rdi,
           regs.r8,
           regs.r9,
           regs.r10,
           regs.r11,
           regs.r12,
           regs.r13,
           regs.r14,
           regs.r15);
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
        printf("Uso: createfd <name>\n");
        return;
    }
    int fd = fd_open(args);
    if (fd < 0)
    {
        printf("Error: no se pudo crear FD '%s'\n", args);
        return;
    }
    printf("FD creado: %d\n", fd);
}

void cmd_writefd(char *args)
{
    if (!args || *args == '\0')
    {
        printf("Uso: writefd <fd> <texto>\n");
        return;
    }
    // split fd and message
    char *fd_str = args;
    char *msg = find_args(args); // mutates: places \0 after fd
    if (!msg || *msg == '\0')
    {
        printf("Uso: writefd <fd> <texto>\n");
        return;
    }
    int fd = strtoint(fd_str);
    int wrote = write(fd, msg, strlen(msg));
    if (wrote < 0)
    {
        printf("Error: write a fd %d\n", fd);
        return;
    }
    printf("Escritos %d bytes en fd %d\n", wrote, fd);
}

void cmd_readfd(char *args)
{
    if (!args || *args == '\0')
    {
        printf("Uso: readfd <fd>\n");
        return;
    }
    int fd = strtoint(args);
    unsigned char buf[256];
    int total = 0;
    while (1)
    {
        int n = read(fd, buf, sizeof(buf) - 1);
        if (n <= 0)
            break;
        buf[n] = '\0';
        // print raw without formatting issues
        shell_print((char *)buf);
        total += n;
    }
    if (total == 0)
    {
        printf("(sin datos)\n");
    }
    else
    {
        printf("\nLeidos %d bytes de fd %d\n", total, fd);
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
    printf("%s\n", fontmanager_get_font_name(font_index));
    shell_newline();
}

void shell_list_fonts()
{
    int count = fontmanager_get_font_count();
    printf("Fuentes disponibles:\n");
    for (int i = 0; i < count; i++)
    {
        const char *name = fontmanager_get_font_name(i);
        printf("  Id: %d  -  ", i);
        printf("%s\n", name);
    }
}

void cmd_fdlist()
{
    fd_info_u_t infos[32];
    int n = fd_list(infos, 32);
    if (n <= 0)
    {
        printf("(sin FDs dinamicos)\n");
        return;
    }
    printf("FDs dinamicos (%d):\n", n);
    for (int i = 0; i < n; i++)
    {
        printf("  id=%d name=%s bytes=%u\n", infos[i].fd, infos[i].name, (unsigned)infos[i].size);
    }
}

void command_switch(char *cmd_copy, char *args)
{
    if (!strcmp(cmd_copy, "help"))
    {
        new_proc((task_fn_t)cmd_help, NULL);
    }
    else if (!strcmp(cmd_copy, "clear"))
    {
        cmd_clear();
    }
    else if (!strcmp(cmd_copy, "mem"))
    {
        new_proc(cmd_mem, NULL);
    }
    else if (!strcmp(cmd_copy, "ps"))
    {
        new_proc(cmd_ps, NULL);
    }
    else if (!strcmp(cmd_copy, "loop"))
    {
        char param[10];
        strcpy(param, args);
        new_proc((task_fn_t)cmd_loop, (void *)param);
    }
    else if (!strcmp(cmd_copy, "kill"))
    {
        cmd_kill(strtoint(args));
    }

    else if (!strcmp(cmd_copy, "nice"))
    {
        cmd_nice(strtoint(args), strtoint(args + 1));
    }
    else if (!strcmp(cmd_copy, "block"))
    {
        cmd_block(strtoint(args));
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
        new_proc(cmd_testMM, args);
    }
    else if (!strcmp(cmd_copy, "test_processes"))
    {
        new_proc(cmd_testProcesses, &args);
    }
    else if (!strcmp(cmd_copy, "test_priority"))
    {
        new_proc(cmd_testPriority, &args);
    }
    else if (!strcmp(cmd_copy, "test_synchro"))
    {
        new_proc(cmd_testSynchro, &args);
    }
    else if (!strcmp(cmd_copy, "test_no_synchro"))
    {
        new_proc(cmd_testNoSynchro, &args);
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
        shell_print_colored("Error: ", ERROR_COLOR);
        printf("Comando desconocido '%s'\n", cmd_copy);

        printf("Escribe 'help' para ver comandos disponibles.\n");
    }
}

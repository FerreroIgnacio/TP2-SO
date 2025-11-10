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

static void *const pongisgolfModuleAddress = (void *)0x8000000;

// Comandos disponibles
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
    printf("  testMM <bytes>   - Ejecuta stress test del manejador de memoria\n");
    printf("  createfd <name>  - Crea FD dinámico y devuelve su id\n");
    printf("  writefd <fd> <text> - Escribe texto ASCII en el FD\n");
    printf("  readfd <fd>      - Lee todos los bytes disponibles y los imprime\n");
    printf("  fdlist           - Lista FDs dinámicos (id, nombre, bytes)\n");
    printf("\nTests disponibles:\n");

    printf("\nProgramas disponibles:\n");
    printf("  pongisgolf\n");
    printf("\nControles:\n");
    printf("  Enter - Ejecutar comando\n");
    printf("  Backspace - Borrar caracter\n");
}

void cmd_clear()
{
    clear_screen();
}

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

void cmd_testMM(char *args)
{
    if (!args)
    {
        printf("Uso: testMM <bytes>\n");
        return;
    }

    while (*args == ' ')
    {
        args++;
    }

    if (*args == '\0')
    {
        printf("Uso: testMM <bytes>\n");
        return;
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
        return;
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
            return;
        }
    }

    printf("Iniciando testMM con %s bytes\n", args);
    char *test_args[] = {args};
    uint64_t result = test_mm(1, test_args);
    printf("testMM finalizado con codigo %x\n", result);

    *arg_end = saved;
}

void cmd_testProcesses(char *args)
{
    printf("Iniciando testProcesses...\n");
    // char *test_args[] = {args};
    uint64_t result = 0; // test_processes(0, test_args);
    printf("testProcesses finalizado con codigo %x\n", result);
}

void cmd_testPriority(char *args)
{
    printf("Iniciando testPriority...\n");
    // char *test_args[] = {args};
    uint64_t result = 0; // test_priority(0, test_args);
    printf("testPriority finalizado con codigo %x\n", result);
}

void cmd_testSynchro(char *args)
{
    printf("Iniciando testSynchro...\n");
    // char *test_args[] = {args};
    uint64_t result = 0; // test_synchronization(0, test_args);
    printf("testSynchro finalizado con codigo %x\n", result);
}

void cmd_testNoSynchro(char *args)
{
    printf("Iniciando testNoSynchro...\n");
    // char *test_args[] = {args};
    uint64_t result = 0; // test_no_synchronization(0, test_args);
    printf("testNoSynchro finalizado con codigo %x\n", result);
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

// TEST:

void command_switch(char *cmd_copy, char *args)
{
    if (!strcmp(cmd_copy, "help"))
    {
        cmd_help();
    }
    else if (!strcmp(cmd_copy, "clear"))
    {
        cmd_clear();
    }
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
    else if (!strcmp(cmd_copy, "testMM"))
    {
        cmd_testMM(args);
    }
    else if (!strcmp(cmd_copy, "testProcesses"))
    {
        cmd_testProcesses(args);
    }
    else if (!strcmp(cmd_copy, "testPriority"))
    {
        cmd_testPriority(args);
    }
    else if (!strcmp(cmd_copy, "testSynchro"))
    {
        cmd_testSynchro(args);
    }
    else if (!strcmp(cmd_copy, "testNoSynchro"))
    {
        cmd_testNoSynchro(args);
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

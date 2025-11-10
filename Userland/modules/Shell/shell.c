#include "../../libs/standard/standard.h"
#include "../../libs/video/video.h"
#include "../../libs/fontManager/fontManager.h"
#include "../../libs/invalidOpCode/invalidOpCode.h"
#include "../../libs/test/test.h"
// #include "../PongisGolf/pongisgolf.h"

#define FONT_BMP_SIZE 8
#define FONT_SIZE 1
#define BUFFER_SIZE 256
#define LINE_Y_PADDING 4
#define LINES_PER_SCREEN height / ((FONT_SIZE * FONT_BMP_SIZE) + LINE_Y_PADDING)
#define STDOUT_BUFFER_SIZE 4096

#define SHELL_COLOR 0x00000A
#define FONT_COLOR 0xAAAAAA
#define ERROR_COLOR 0xAA4444
#define PROMPT_COLOR 0x44AAA4

#define KEYS_PER_LOOP 8

#define LSHIFT_MAKECODE 0x2A
#define RSHIFT_MAKECODE 0x36
#define LCTRL_MAKECODE 0x1D
#define LALT_MAKECODE 0x38
#define TAB_MAKECODE 0x0F
#define ESC_MAKECODE 0x01

// Variables globales
static frameBuffer frame;
static unsigned char command_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static int cursor_x = 0;
static int cursor_y = 0;
char firstEntry = 1;

// Prototipos de funciones
void shell_main();
void handle_keyboard_input();
void shell_putchar(unsigned char c);
void shell_print(const char *str);
void shell_print_colored(const char *str, uint32_t color);
void shell_newline();
void shell_print_prompt();
void clear_buffer();
void clear_screen();
void hide_cursor();
void execute_command();

// Comandos disponibles
void cmd_help();
void cmd_clear();
void cmd_echo(char *args);
void cmd_test();
void cmd_info();
void cmd_dateTime();
void cmd_registers();
void cmd_test0Div();
void cmd_testMM(char *args);
// New FD commands
void cmd_createfd(char *args);
void cmd_writefd(char *args);
void cmd_readfd(char *args);
void cmd_fdlist();

// Utilidades de string
char *find_args(char *cmd);

// programas
static void *const pongisgolfModuleAddress = (void *)0x8000000;
typedef int (*EntryPoint)();

char *find_args(char *cmd)
{
    while (*cmd && *cmd != ' ')
        cmd++;
    if (*cmd == ' ')
    {
        *cmd++ = '\0';
        while (*cmd == ' ')
            cmd++;
    }
    return cmd;
}

// Función para imprimir un carácter
void shell_putchar(unsigned char c)
{
    if (c == '\n')
    {
        shell_newline();
        return;
    }
    frameDrawChar(frame, c, FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
    font_info_t currentFont = fontmanager_get_current_font();
    cursor_x += FONT_SIZE * currentFont.width;

    if (cursor_x >= frameGetWidth(frame))
    {
        shell_newline();
    }
}

// Función para imprimir string
void shell_print(const char *str)
{
    shell_print_colored(str, FONT_COLOR);
}

// Función para imprimir string con color específico
void shell_print_colored(const char *str, uint32_t color)
{
    while (*str)
    {
        if (*str == '\n')
        {
            shell_newline();
        }
        else
        {
            frameDrawChar(frame, *str, color, SHELL_COLOR, cursor_x, cursor_y);
            font_info_t currentFont = fontmanager_get_current_font();
            cursor_x += FONT_SIZE * currentFont.width;
            if (cursor_x >= frameGetWidth(frame))
            {
                shell_newline();
            }
        }
        str++;
    }
}

// Nueva línea
void shell_newline()
{
    hide_cursor();
    cursor_x = 0;
    font_info_t currentFont = fontmanager_get_current_font();
    cursor_y += FONT_SIZE * currentFont.height + LINE_Y_PADDING;

    // Si llegamos al final de la pantalla, hacer scroll
    if (cursor_y >= frameGetHeight(frame) - currentFont.height)
    {
        clear_screen();
        cursor_y = 0;
        shell_print_colored("--- Se limpio la pantalla (scroll) ---\n", PROMPT_COLOR);
    }
}

// Mostrar prompt
void shell_print_prompt()
{
    if (cursor_x != 0)
    {
        shell_newline();
    }
    shell_print_colored("> ", PROMPT_COLOR);
}

// Limpiar buffer de comandos
void clear_buffer()
{
    buffer_pos = 0;
    command_buffer[0] = '\0';
}

// Limpiar pantalla
void clear_screen()
{
    frameFill(frame, SHELL_COLOR);
    cursor_x = cursor_y = 0;
}
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
    printf("testMM finalizado con codigo %x\n", (int)result);

    *arg_end = saved;
}

// New: createfd <name>
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

// New: writefd <fd> <text>
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

// New: readfd <fd>
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

// New: list dynamic FDs
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

// Crea la funcion para cambiar de fuente
// Crea la funcion para cambiar de fuente
//  Cambia la fuente actual por el índice dado
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
// Muestra las fuentes disponibles
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
// Ejecutar comando
void execute_command()
{
    hide_cursor();
    if (buffer_pos == 0)
        return;

    // Borro el cursor antes de ejecutar comando
    frameDrawChar(frame, ' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);

    command_buffer[buffer_pos] = '\0';

    char cmd_copy[BUFFER_SIZE];
    strcpy(cmd_copy, (char *)command_buffer);
    char *args = find_args(cmd_copy);

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

static int cursor_visible = 1;
static int last_cursor_time = 0;
static int cursor_drawn = 0;
#define CURSOR_BLINK_INTERVAL 150 // en ticks

void update_cursor()
{
    int current_time = getBootTime();
    int should_blink = (current_time - last_cursor_time >= CURSOR_BLINK_INTERVAL);

    if (should_blink)
    {
        cursor_visible = !cursor_visible;
        last_cursor_time = current_time;
    }

    // Solo actualizar la pantalla si el estado visual cambió
    int should_be_drawn = cursor_visible;
    if (cursor_drawn != should_be_drawn)
    {
        char cursor_char = should_be_drawn ? '_' : ' ';
        frameDrawChar(frame, cursor_char, PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
        cursor_drawn = should_be_drawn;
    }
}

// Función para forzar cursor visible (llamar después de escribir)
void reset_cursor()
{
    cursor_visible = 1;
    last_cursor_time = getBootTime();
    if (!cursor_drawn)
    {
        frameDrawChar(frame, '_', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
        cursor_drawn = 1;
    }
}

// Función para ocultar cursor (llamar antes de escribir texto)
void hide_cursor()
{
    if (cursor_drawn)
    {
        frameDrawChar(frame, ' ', SHELL_COLOR, SHELL_COLOR, cursor_x, cursor_y);
        cursor_drawn = 0;
    }
}

// Manejar entrada del teclado usando syscalls
void handle_keyboard_input()
{
    update_cursor();
    // Actualizar estado de shift usando isKeyPressed
    unsigned char c;
    if ((c = getchar()) > 0)
    {
        // Manejar teclas especiales
        if (c == '\n')
        { // Enter
            shell_newline();
            execute_command();
            clear_buffer();
            shell_print_prompt();
            return;
        }
        if (c == '\b')
        { // Backspace
            if (buffer_pos > 0)
            {
                buffer_pos--;
                hide_cursor();
                font_info_t currentFont = fontmanager_get_current_font();

                command_buffer[buffer_pos] = '\0';
                if (cursor_x >= FONT_SIZE * currentFont.width)
                {
                    cursor_x -= FONT_SIZE * currentFont.width;
                    frameDrawChar(frame, ' ', FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
                }
            }
            return;
        }

        // Solo agregar caracteres imprimibles
        if (c && (buffer_pos < (BUFFER_SIZE - 1)))
        {
            command_buffer[buffer_pos++] = c;
            shell_putchar(c);
        }
    }
}
// shell at 0x400000
void shell_welcome()
{
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL v16.1\n", PROMPT_COLOR);
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    printf("Escribe 'help' para ver comandos disponibles.\n\n");
}

int main()
{

    if (firstEntry)
    {
        frame = getFB();
        if (!frame)
        {
            return -1;
        }
        clear_screen();
        clear_buffer();
        fontmanager_set_font(1);
        shell_welcome();
        shell_print_prompt();
        firstEntry = 0;
    }

    while (1)
    {
        unsigned char stdoutBuff[STDOUT_BUFFER_SIZE];
        read(STDOUT, stdoutBuff, STDOUT_BUFFER_SIZE);
        if (strlen((char *)stdoutBuff) > 0)
        {
            cursor_x = 0;
            shell_print((char *)stdoutBuff);
            shell_print_prompt();
        }

        for (int i = 0; i < KEYS_PER_LOOP; i++)
            handle_keyboard_input();

        setFB(frame);
    }

    return 0;
}

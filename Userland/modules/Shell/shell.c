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
#include "./shell_commands.h"

// Variables globales
frameBuffer frame;
unsigned char command_buffer[BUFFER_SIZE];
int buffer_pos = 0;
int cursor_x = 0;
int cursor_y = 0;
static char firstEntry = 1;

// Prototipos de funciones
static void execute_command();
static void handle_keyboard_input();

extern void cmd_echo(char *args);
// Ejecutar comando
static void execute_command()
{
    hide_cursor();
    if (buffer_pos == 0)
        return;

    // Borro el cursor antes de ejecutar comando
    frameDrawChar(frame, ' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);

    command_buffer[buffer_pos] = '\0';

    // Detectar pipeline "proc1 | proc2"
    char line[BUFFER_SIZE];
    strcpy(line, (char *)command_buffer);
    char *bar = NULL;
    for (char *p = line; *p; ++p)
    {
        if (*p == '|')
        {
            bar = p;
            break;
        }
    }
    if (bar)
    {
        // Split and trim both sides
        *bar = '\0';
        char *left = line;
        char *right = bar + 1;
        while (*left == ' ')
            left++;
        while (*right == ' ')
            right++;
        // rtrim left
        char *lend = left + strlen(left);
        while (lend > left && (lend[-1] == ' '))
        {
            lend--;
        }
        *lend = '\0';
        // rtrim right
        char *rend = right + strlen(right);
        while (rend > right && (rend[-1] == ' '))
        {
            rend--;
        }
        *rend = '\0';

        if (*left == '\0' || *right == '\0')
        {
            shell_print_colored("Error de sintaxis en pipeline\n", ERROR_COLOR);
            return;
        }
        // Parse name/args for left and right
        char left_copy[BUFFER_SIZE];
        strcpy(left_copy, left);
        char right_copy[BUFFER_SIZE];
        strcpy(right_copy, right);
        char *left_args = find_args(left_copy);
        char *right_args = find_args(right_copy);

        // Si el comando izquierdo es un builtin soportado (echo), no spawnear proceso
        if (!strcmp(left_copy, "echo"))
        {
            int pipe_id = pipe_create();
            if (pipe_id < 0)
            {
                shell_print_colored("No se pudo crear pipe\n", ERROR_COLOR);
                return;
            }
            int pid2 = shell_launch_program(right_copy, right_args);
            if (pid2 < 0)
            {
                shell_print_colored("Programa der. desconocido\n", ERROR_COLOR);
                return;
            }
            // Conectar stdin del consumidor y stdout de la shell al pipe
            fd_bind_std(pid2, 0 /*STDIN*/, pipe_id);
            fd_bind_std(getpid(), 1 /*STDOUT*/, pipe_id);
            // Ejecutar el builtin para volcar su salida al pipe
            cmd_echo(left_args ? left_args : "");
            // Restaurar stdout de la shell
            fd_bind_std(getpid(), 1 /*STDOUT*/, -1);
            return;
        }

        int pipe_id = pipe_create();
        if (pipe_id < 0)
        {
            shell_print_colored("No se pudo crear pipe\n", ERROR_COLOR);
            return;
        }

        int pid1 = shell_launch_program(left_copy, left_args);
        if (pid1 < 0)
        {
            shell_print_colored("Programa izq. desconocido\n", ERROR_COLOR);
            return;
        }
        int pid2 = shell_launch_program(right_copy, right_args);
        if (pid2 < 0)
        {
            shell_print_colored("Programa der. desconocido\n", ERROR_COLOR);
            return;
        }

        // Vincular stdout de proc1 -> pipe, stdin de proc2 <- pipe
        fd_bind_std(pid1, 1 /*STDOUT*/, pipe_id);
        fd_bind_std(pid2, 0 /*STDIN*/, pipe_id);
        return;
    }

    char cmd_copy[BUFFER_SIZE];
    strcpy(cmd_copy, (char *)command_buffer);
    char *args = find_args(cmd_copy);

    command_switch(cmd_copy, args);
}

// Manejar entrada del teclado usando syscalls
static void handle_keyboard_input()
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

static void shell_welcome()
{
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL\n", PROMPT_COLOR);
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
        int status;
        waitpid(0, &status, WNOHANG);
    }

    return 0;
}

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

        int pipe_id = pipe_create();
        if (pipe_id < 0)
        {
            shell_print_colored("No se pudo crear pipe\n", ERROR_COLOR);
            return;
        }

        int pid_right = shell_launch_program(right_copy, right_args);
        if (pid_right < 0)
        {
            shell_print_colored("Programa der. desconocido\n", ERROR_COLOR);
            return;
        }
        fd_bind_std(pid_right, 0 /*STDIN*/, pipe_id);

        // Manejar proceso izquierdo (builtin echo especial) o programa externo
        if (!strcmp(left_copy, "echo"))
        {
            // lanzar proceso echo para escribir en el pipe
            extern int echo_proc(void *);
            // duplicar args (puede ser NULL)
            char *dup = NULL;
            if (left_args && *left_args)
            {
                int len = strlen(left_args);
                dup = (char *)malloc(len + 1);
                if (dup)
                {
                    memcpy(dup, left_args, len + 1);
                }
            }
            int pid_echo = new_proc((task_fn_t)echo_proc, dup);
            if (pid_echo < 0)
            {
                shell_print_colored("No se pudo lanzar echo\n", ERROR_COLOR);
                if (dup)
                    free(dup);
                return;
            }
            fd_bind_std(pid_echo, 1 /*STDOUT*/, pipe_id);
        }
        else
        {
            int pid_left = shell_launch_program(left_copy, left_args);
            if (pid_left < 0)
            {
                shell_print_colored("Programa izq. desconocido\n", ERROR_COLOR);
                return;
            }
            fd_bind_std(pid_left, 1 /*STDOUT*/, pipe_id);
        }
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
    unsigned char c = getchar(); // bloqueante
    if (c == '\n')
    {
        shell_newline();
        execute_command();
        clear_buffer();
        shell_print_prompt();
        reset_cursor();
        return;
    }
    if (c == '\b')
    {
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
            reset_cursor();
        }
        return;
    }
    if (buffer_pos < (BUFFER_SIZE - 1))
    {
        command_buffer[buffer_pos++] = c;
        command_buffer[buffer_pos] = '\0';
        shell_putchar(c);
        reset_cursor();
    }
}

static void shell_welcome()
{
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL\n", PROMPT_COLOR);
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
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
        // Aumentar prioridad de la shell para evitar que quede bloqueada por procesos hijos.
        set_priority(getpid(), 0); // asumir 0 = mayor prioridad
        firstEntry = 0;
    }
    while (1)
    {
        setFB(frame);
        handle_keyboard_input();
    }
    return 0;
}

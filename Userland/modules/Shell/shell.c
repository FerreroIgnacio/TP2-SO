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

// Variables globales (buffer de linea ahora reside en FD dinamico shell_cmd_fd)
frameBuffer frame;
int buffer_pos = 0; // longitud actual de la linea en FD shell_cmd_fd
int cursor_x = 0;
int cursor_y = 0;
static char firstEntry = 1;
int shell_cmd_fd = -1; // FD dinamico usado como buffer de linea (>=3), expuesto para clear_buffer
int pipes_to_print[MAX_PROC + 1] = {0};
int pipes_present[MAX_PROC] = {0};

// Prototipos
static void execute_command_line(const char *line);
static void handle_stdin_chunk();
static void rebuild_line_visual();
static int read_fd_size(int fd);
static int read_fd_snapshot(int fd, unsigned char *buf, int max);

static void clean_EOT(char *string)
{
    if (string == NULL)
        return;

    int read_pos = 0;
    int write_pos = 0;

    while (string[read_pos] != '\0')
    {
        if (string[read_pos] != EOT)
        {
            string[write_pos] = string[read_pos];
            write_pos++;
        }
        read_pos++;
    }

    string[write_pos] = '\0';
}

// Ejecutar comando dado un string completo
static void execute_command_line(const char *line)
{
    hide_cursor();
    if (line == NULL || *line == '\0')
        return;
    // Borro cursor visual
    frameDrawChar(frame, ' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
    // Copiar linea local para parseo (pipeline etc.)
    char work[BUFFER_SIZE] = {0};
    strncpy(work, line, BUFFER_SIZE - 1);
    char *tokens[100] = {0};

    // Separar el string por espacios
    int token_count = 0;
    char *token = strtok(work, " ");

    int foreground_mode = 1; // por defecto en foreground
    while (token != NULL && token_count < 100)
    {
        tokens[token_count] = token;
        token_count++;
        token = strtok(NULL, " ");
    }

    if (token_count == 0)
        return;

    if (strcmp(tokens[token_count - 1], "&") == 0)
    {
        foreground_mode = 0; // modo background
        tokens[token_count - 1] = NULL;
        token_count--;
    }

    // Detectar pipes
    int left_pipe_args = 0;
    int right_pipe_args = -1;
    for (int i = 0; i < token_count; i++)
    {
        if (strcmp(tokens[i], "|") == 0)
        {
            right_pipe_args = i + 1; // comando derecho
            tokens[i] = NULL;        // separar comandos
        }
    }
    // Ejecutar comando tokenizado
    execute_tokenized_command(tokens, token_count, foreground_mode, left_pipe_args, right_pipe_args);
}

// Reconstruye visualmente la linea actual leyendo FD 3
static void rebuild_line_visual()
{
    font_info_t currentFont = fontmanager_get_current_font();
    int char_width = FONT_SIZE * currentFont.width;
    int saved_y = cursor_y;
    for (int x = 0; x < frameGetWidth(frame); x += char_width)
    {
        frameDrawChar(frame, ' ', SHELL_COLOR, SHELL_COLOR, x, saved_y);
    }
    cursor_x = 0;
    cursor_y = saved_y;
    shell_print_colored("> ", PROMPT_COLOR);
    unsigned char temp[BUFFER_SIZE];
    int total = read_fd_snapshot(shell_cmd_fd, temp, BUFFER_SIZE - 1);
    if (total > 0)
    {
        // Restaurar contenido porque snapshot consumió los bytes
        write(shell_cmd_fd, (char *)temp, total);
        buffer_pos = total;
        temp[total] = '\0';
        for (int i = 0; i < total; i++)
        {
            shell_putchar(temp[i]);
        }
    }
    else
    {
        buffer_pos = 0;
    }
    reset_cursor();
}

// Maneja lectura parcial desde STDIN y actualiza FD 3

static void handle_stdin_chunk(char *inbuf, int size)
{
    update_cursor();
    int n = size;
    if (n <= 0)
        return;

    // eliminar el EOT
    int w = 0;
    for (int r = 0; r < n; r++)
    {
        if (inbuf[r] != EOT)
        {
            inbuf[w++] = inbuf[r];
        }
    }
    n = w;

    for (int i = 0; i < n; i++)
    {
        unsigned char c = inbuf[i];
        if (c == '\n')
        {
            unsigned char linebuf[BUFFER_SIZE];
            int total = read_fd_snapshot(shell_cmd_fd, linebuf, BUFFER_SIZE - 1);
            if (total > 0)
            {
                linebuf[total] = '\0';
            }
            else
            {
                linebuf[0] = '\0';
            }
            buffer_pos = 0; // FD quedo vacio
            shell_newline();

            execute_command_line((char *)linebuf);

            reset_cursor();
            continue;
        }
        if (c == '\b')
        {
            if (buffer_pos > 0)
            {
                unsigned char temp[BUFFER_SIZE];
                int total = read_fd_snapshot(shell_cmd_fd, temp, BUFFER_SIZE - 1);
                if (total > 0)
                {
                    total--;
                }
                // Quitar flush: ya consumimos el contenido, solo reescribir truncado
                if (total > 0)
                {
                    write(shell_cmd_fd, (char *)temp, total);
                }
                buffer_pos = total;
                rebuild_line_visual();
            }
            continue;
        }
        if (buffer_pos < BUFFER_SIZE - 1)
        {
            write(shell_cmd_fd, (char *)&c, 1);
            buffer_pos++;
            shell_putchar(c);
            reset_cursor();
        }
    }
}

static void shell_welcome()
{
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL\n", PROMPT_COLOR);
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
}

int get_shell_stdout_pipe()
{
    for (int i = 1; i < MAX_PROC; i++)
    {
        if (!pipes_present[i])
        {
            pipes_present[i] = 1;
            return pipes_to_print[i];
        }
    }
    return -1;
}

int last_readed_pipe = 0;
int main()
{

    if (firstEntry)
    {
        frame = getFB();
        if (!frame)
            return -1;
        clear_screen();
        fontmanager_set_font(1);
        shell_welcome();
        shell_print_prompt();

        shell_cmd_fd = fd_open("shellcmd");
        if (shell_cmd_fd < 0)
        {
            shell_print_colored("Error creando FD shellcmd\n", ERROR_COLOR);
        }
        firstEntry = 0;
        buffer_pos = 0;

        for (int i = 1; i < MAX_PROC; i++)
        {
            pipes_to_print[i] = pipe_create();
            pipes_present[i] = 0;
        }
        pipes_to_print[MAX_PROC] = pipe_create();
        if (pipes_to_print[MAX_PROC] > 0)
        {
            fd_bind_std(getpid(), STDOUT, pipes_to_print[MAX_PROC]);
        }
    }
    while (1)
    {
        setFB(frame);
        int pipe_to_print = select(pipes_to_print, last_readed_pipe, MAX_PROC + 1);
        char buffer[STD_BUFF_SIZE];
        if (pipe_to_print > 0)
        {
            int readed = pipe_read(pipes_to_print[pipe_to_print], buffer, STD_BUFF_SIZE);
            buffer[readed] = '\0';
            clean_EOT(buffer);
            shell_print(buffer);
            for (int i = 0; i < readed; i++)
            {
                if (buffer[i] == EOT)
                {
                    pipes_present[pipe_to_print] = 0;
                    break;
                }
            }
            /* versión 2, se evita el for:
            if (buffer[readed - 1] == EOT)
            {
                pipes_present[pipe_to_print] = 0;
            }
            */
        }
        else if (pipe_to_print == STDIN && get_left_fg_proc() == getpid())
        {
            int readed = pipe_read(pipe_to_print, buffer, STD_BUFF_SIZE);
            handle_stdin_chunk(buffer, readed);
        }

        pid_t left_fg_proc = get_left_fg_proc();
        pid_t right_fg_proc = get_right_fg_proc();

        if (left_fg_proc != getpid())
        {
            if (right_fg_proc <= 1)
            {
                int status;
                if (waitpid(left_fg_proc, &status, WNOHANG) > 0)
                {
                    fd_bind_std(getpid(), STDIN, STDIN); // shell toma nuevamente el control de la shell

                    set_left_fg_proc(getpid());
                    printf("\nProceso %d finalizado con estado %d\n", left_fg_proc, status);
                    shell_print_prompt();
                    rebuild_line_visual();
                }
            }
            else
            {
                int status_left, status_right;
                int left_done = waitpid(left_fg_proc, &status_left, WNOHANG) > 0;
                int right_done = waitpid(right_fg_proc, &status_right, WNOHANG) > 0;

                if (left_done && right_done)
                {
                    fd_bind_std(getpid(), STDIN, STDIN);
                    set_left_fg_proc(getpid());
                    set_right_fg_proc(-1);
                    printf("\nProceso %d finalizado con estado %d\n", right_fg_proc, status_right);
                    shell_print_prompt();
                    rebuild_line_visual();
                }
                else if (!left_done && right_done)
                {
                    kill(left_fg_proc);
                    fd_bind_std(getpid(), STDIN, STDIN);
                    set_left_fg_proc(getpid());
                    set_right_fg_proc(-1);
                    printf("\nProceso %d finalizado con estado %d\n", right_fg_proc, status_right);
                    shell_print_prompt();
                    rebuild_line_visual();
                }
                else if (left_done && !right_done)
                {
                    set_left_fg_proc(-1);
                }
            }
        }
        yield(); // permite que corra el resto de procesos.
    }
    return 0;
}

static int read_fd_size(int fd)
{
    fd_info_u_t infos[32];
    int n = fd_list(infos, 32);
    if (n <= 0)
        return 0;
    for (int i = 0; i < n; i++)
    {
        if (infos[i].fd == fd)
            return infos[i].size;
    }
    return 0;
}
static int read_fd_snapshot(int fd, unsigned char *buf, int max)
{
    int sz = read_fd_size(fd);
    if (sz <= 0)
        return 0;
    if (sz > max)
        sz = max;
    int r = read(fd, buf, sz);
    if (r < 0)
        return 0;
    return r;
}

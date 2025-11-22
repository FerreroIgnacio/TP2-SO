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
#include "./commands/commands.h"

// Variables globales (buffer de linea ahora reside en FD dinamico shell_cmd_fd)
frameBuffer frame;
int buffer_pos = 0; // longitud actual de la linea en FD shell_cmd_fd
int cursor_x = 0;
int cursor_y = 0;
static char firstEntry = 1;
int shell_cmd_fd = -1; // FD dinamico usado como buffer de linea (>=3), expuesto para clear_buffer

// Prototipos
static void execute_command_line(const char *line);
static void handle_stdin_chunk();
static void rebuild_line_visual();

// Ejecutar comando dado un string completo
static void execute_command_line(const char *line)
{
    hide_cursor();
    if (line == NULL || *line == '\0')
        return;
    // Borro cursor visual
    frameDrawChar(frame, ' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y);
    // Copiar linea local para parseo (pipeline etc.)
    char work[BUFFER_SIZE];
    work[0] = '\0';
    strncpy(work, line, BUFFER_SIZE - 1);
    work[BUFFER_SIZE - 1] = '\0';
    // Detectar pipeline
    char *bar = NULL;
    for (char *p = work; *p; ++p)
    {
        if (*p == '|')
        {
            bar = p;
            break;
        }
    }
    if (bar)
    {
        *bar = '\0';
        char *left = work;
        char *right = bar + 1;
        while (*left == ' ')
            left++;
        while (*right == ' ')
            right++;
        char *lend = left + strlen(left);
        while (lend > left && lend[-1] == ' ')
            lend--;
        *lend = '\0';
        char *rend = right + strlen(right);
        while (rend > right && rend[-1] == ' ')
            rend--;
        *rend = '\0';
        if (*left == '\0' || *right == '\0')
        {
            shell_print_colored("Error de sintaxis en pipeline\n", ERROR_COLOR);
            return;
        }
        char left_copy[BUFFER_SIZE];
        strncpy(left_copy, left, BUFFER_SIZE - 1);
        left_copy[BUFFER_SIZE - 1] = '\0';
        char right_copy[BUFFER_SIZE];
        strncpy(right_copy, right, BUFFER_SIZE - 1);
        right_copy[BUFFER_SIZE - 1] = '\0';
        char *left_args = find_args(left_copy);
        char *right_args = find_args(right_copy);
        int pipe_id = pipe_create();
        if (pipe_id < 0)
        {
            shell_print_colored("No se pudo crear pipe\n", ERROR_COLOR);
            return;
        }
        int pid_right = -1; // shell_launch_program(right_copy, right_args);
        if (pid_right < 0)
        {
            shell_print_colored("Programa der. desconocido\n", ERROR_COLOR);
            return;
        }
        fd_bind_std(pid_right, 0, pipe_id);
        if (!strcmp(left_copy, "echo"))
        {
            extern int echo_proc(void *);
            char *dup = NULL;
            if (left_args && *left_args)
            {
                size_t len = strlen(left_args);
                dup = malloc(len + 1);
                if (dup)
                    memcpy(dup, left_args, len + 1);
            }
            int pid_echo = new_proc((task_fn_t)echo_proc, dup);
            if (pid_echo < 0)
            {
                shell_print_colored("No se pudo lanzar echo\n", ERROR_COLOR);
                if (dup)
                    free(dup);
                return;
            }
            fd_bind_std(pid_echo, 1, pipe_id);
        }
        else
        {
            int pid_left = -1; // shell_launch_program(left_copy, left_args);
            if (pid_left < 0)
            {
                shell_print_colored("Programa izq. desconocido\n", ERROR_COLOR);
                return;
            }
            fd_bind_std(pid_left, 1, pipe_id);
        }
        return;
    }
    char cmd_copy[BUFFER_SIZE];
    strncpy(cmd_copy, line, BUFFER_SIZE - 1);
    cmd_copy[BUFFER_SIZE - 1] = '\0';
    char *args = find_args(cmd_copy);
    command_switch(cmd_copy, args);
    rebuild_line_visual();
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
    int total = read(shell_cmd_fd, temp, BUFFER_SIZE - 1);
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
/*
// Maneja lectura parcial desde STDIN y actualiza FD 3
static void handle_stdin_chunk()
{
    update_cursor();
    unsigned char inbuf[FD_SIZE];
    int n = read(STDIN, inbuf, sizeof(inbuf));
    if (n <= 0)
        return;

    for (int i = 0; i < n; i++)
    {
        unsigned char c = inbuf[i];
        if (c == '\n')
        {
            unsigned char linebuf[BUFFER_SIZE];
            int total = read(shell_cmd_fd, linebuf, BUFFER_SIZE - 1);
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

            if (get_foreground_proc() == getpid()) // si shell es el proc en fg
            {
                execute_command_line((char *)linebuf);
            }

            reset_cursor();
            continue;
        }
        if (c == '\b')
        {
            if (buffer_pos > 0)
            {
                unsigned char temp[BUFFER_SIZE];
                int total = read(shell_cmd_fd, temp, BUFFER_SIZE - 1);
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

    pid_t fg_proc = get_foreground_proc();
    if (!(fg_proc == getpid())) // si shell es el proc en fg
    {
        int status;
        if (waitpid(fg_proc, &status, WNOHANG) > 0)
        {
            // volcar salida pendiente del proceso foreground
            flush_foreground_output();
            fd_bind_std(getpid(), STDIN, STDIN);
            set_foreground_proc(getpid());
            printf("\nProceso %d finalizado con estado %d\n", fg_proc, status);
         //   shell_print_prompt();
        }
        else {
            // Si sigue vivo, intentar mostrar salida parcial (streaming)
            flush_foreground_output();
        }
    }
    else {
        // Shell en foreground: limpiar cualquier residuo del pipe previo
        flush_foreground_output();
    }
}
*/
static void shell_welcome()
{
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL\n", PROMPT_COLOR);
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
}


char lineBuffer[LINE_BUFFER_SIZE];
int lineBufferDim = 0;

// '\b' y '\n' los manejas nativamente el main principal
char metaChars[] = {'|', '>', '&'};
char metaChartsDim = 3;

int execute(char * cmd_line) {
 //   run_in_foreground((task_fn_t)cmd_help_run, NULL);
 //   return 1;
    if (cmd_line == NULL || *cmd_line == '\0')
        return -2;

    // Copiar línea para parseo
    char cmd_copy[BUFFER_SIZE];
    strncpy(cmd_copy, cmd_line, BUFFER_SIZE - 1);
    cmd_copy[BUFFER_SIZE - 1] = '\0';

    // Parsear palabras
    char *arr[100];  // Array de punteros, no matriz
    int i = 0;

    char *word = strtok(cmd_copy, " ");  // strtok, no strok
    while (word != NULL && i < 100) {
        int isMeta = 0;
        for (int j = 0; j < metaChartsDim; j++) {
            // Comparar carácter (asumiendo metaChars es char[])
            if (word[0] == metaChars[j] && word[1] == '\0') {
                isMeta = 1;
                break;
            }
        }

        arr[i] = word;  // Guardar puntero
        word = strtok(NULL, " ");
        i++;
    }

    // Debug
    for (int j = 0; j < i; j++) {
        fprintf(3, "Arg %d: %s\n", j, arr[j]);
    }
    flush_foreground_output();

    // Ejecutar (pero cmd_copy ya fue modificado por strtok)
    return command_switch(arr[0], arr[1]);  // Ajustar según necesites
}
int splitByMetaChars(char *cmd_line, char **commands,int maxCommands, int maxCommandLength)
{
    int cmd_count = 0;
    char *start = cmd_line;
    char *current = cmd_line;
    while (*current != '\0') {
        int isMeta = 0;
        for (int j = 0; j < metaChartsDim; j++) {
            if (*current == metaChars[j]) {
                isMeta = 1;
                break;
            }
        }
        if (isMeta) {
            int segLen = (int)(current - start);
            if (segLen > maxCommandLength)
                return -2; // segment too long
            if (cmd_count >= maxCommands)
                return -1; // too many commands
            *current = '\0';
            commands[cmd_count++] = start;
            start = current + 1;
        }
        current++;
    }
    if (start < current) {
        int segLen = (int)(current - start);
        if (segLen > maxCommandLength)
            return -2;
        if (cmd_count >= maxCommands)
            return -1;
        commands[cmd_count++] = start;
    }
    return cmd_count;
}

int removeBackspaceAndPrevious(unsigned char *buf, int n) {
    int write_pos = 0;

    for (int i = 0; i < n; i++) {
        if (buf[i] == '\b') {
            if (write_pos > 0) {
                write_pos--;  // Retroceder (borra el anterior)
            }
        } else {
            buf[write_pos++] = buf[i];  // Copiar carácter
        }
    }

    return write_pos;  // Retorna nuevo tamaño
}

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
//        shell_print_prompt();

        shell_cmd_fd = fd_open("shellcmd");
        if (shell_cmd_fd < 0)
        {
            shell_print_colored("Error creando FD shellcmd\n", ERROR_COLOR);
        }
        firstEntry = 0;
        buffer_pos = 0;
    }



    while (1) {
        setFB(frame);
        unsigned char buf[FD_SIZE];
        int n = read(STDIN, buf, FD_SIZE); // bloqueante hasta nuevo input

        for(int i = 0; i < n; i++ ){
            switch (buf[i]) {
                case '\b':
                    if (lineBufferDim > 0) {
                        lineBufferDim--;
                        lineBuffer[lineBufferDim] = '\0';

                        frameDrawChar(frame, ' ', SHELL_COLOR, SHELL_COLOR, cursor_x - FONT_SIZE * fontmanager_get_current_font().width, cursor_y);
                        cursor_x -= FONT_SIZE * fontmanager_get_current_font().width;

                        if (cursor_x < PROMPT_LEN + 1 * FONT_SIZE * fontmanager_get_current_font().width) {
                            cursor_y -= FONT_SIZE * fontmanager_get_current_font().height + LINE_Y_PADDING;
                            cursor_x = frameGetWidth(frame) - FONT_SIZE * fontmanager_get_current_font().width;
                        }

                    }
                    break;
                case '\n':
                    //Consumo si quedo algun stray char en el render fd para no bloquear accidentalmente (ya que lineBufferDim puede ser hasta LINE_BUFFER_SIZE = FD_SIZE)
                //    consume_render_fd();
                  //  write(RENDER_FD, lineBuffer, lineBufferDim);
                    //Ahora si consumo y renderizo mi comando
                 //   consume_render_fd();
                    int newLineOnly = lineBufferDim == 0;

                    if (lineBufferDim < LINE_BUFFER_SIZE - 1) {
                        //lineBuffer[lineBufferDim] = buf[i];
                        //lineBufferDim++;
                        //lineBuffer[lineBufferDim] = '\0';
                        shell_putchar(buf[i]);
                    }
                    if(!newLineOnly) {
                        int code = execute(lineBuffer);
                        if (code)
                        {
                            fprintf(RENDER_FD, "Error %d ejecutando comando\n", code);
                            consume_render_fd();
                        }
                    }
                    lineBufferDim = 0;
                    lineBuffer[0] = '\0';

                    break;
                default:
                    if (lineBufferDim < LINE_BUFFER_SIZE - 1) {
                        lineBuffer[lineBufferDim] = buf[i];
                        lineBufferDim++;
                        lineBuffer[lineBufferDim] = '\0';
                        shell_putchar(buf[i]);
                    }
            }


        }

    }
    return 0;
}

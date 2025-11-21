#include "./shell_render.h"
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
#include "./shell_commands.h"

extern frameBuffer frame;
extern int buffer_pos;
extern int cursor_x;
extern int cursor_y;
extern int shell_cmd_fd; // FD usado como buffer de linea
static int cursor_visible = 1;
static int last_cursor_time = 0;
static int cursor_drawn = 0;

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
    if(shell_cmd_fd >= 0){
        flush(shell_cmd_fd); // vaciar contenido del FD 3
    }
}

// Limpiar pantalla
void clear_screen()
{
    frameFill(frame, SHELL_COLOR);
    cursor_x = cursor_y = 0;
}

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

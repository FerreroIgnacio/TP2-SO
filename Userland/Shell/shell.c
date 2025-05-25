
#include "../standard.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CHAR_PER_LINE SCREEN_WIDTH / (FONT_SIZE * FONT_BMP_SIZE)
#define FONT_BMP_SIZE 8 
#define FONT_SIZE 2
#define BUFFER_SIZE 256
#define LINE_Y_PADDING 4
#define LINES_PER_SCREEN SCREEN_HEIGHT / ((FONT_SIZE * FONT_BMP_SIZE) + LINE_Y_PADDING)

#define SHELL_COLOR 0x000008
#define FONT_COLOR 0xAAAAAA
#define ERROR_COLOR 0xAA4444
#define PROMPT_COLOR 0x44AAA4
// Variables globales
static char command_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static int cursor_x = 0;
static int cursor_y = 0;

// Prototipos de funciones
void shell_main();
void handle_keyboard_input();
void shell_putchar(char c);
void shell_print(const char* str);
void shell_print_colored(const char* str, uint32_t color);
void shell_newline();
void shell_print_prompt();
void clear_buffer();
void clear_screen();
void execute_command();

// Comandos disponibles
void cmd_help();
void cmd_clear();
void cmd_echo(char* args);
void cmd_test();
void cmd_info();

// Utilidades de string
int str_equals(const char* s1, const char* s2);
int str_length(const char* str);
char* find_args(char* cmd);
void str_copy(char* dest, const char* src);

// Implementación de funciones de utilidad de string
int str_equals(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

int str_length(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void str_copy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

char* find_args(char* cmd) {
    while (*cmd && *cmd != ' ') cmd++;
    if (*cmd == ' ') {
        *cmd++ = '\0';
        while (*cmd == ' ') cmd++;
    }
    return cmd;
}

// Función para imprimir un carácter
void shell_putchar(char c) {
    if (c == '\n') {
        shell_newline();
        return;
    }
    
    drawChar(c, FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
    cursor_x += FONT_SIZE * FONT_BMP_SIZE;
    
    if (cursor_x >= CHAR_PER_LINE * FONT_SIZE * FONT_BMP_SIZE) {
        shell_newline();
    }
}

// Función para imprimir string
void shell_print(const char* str) {
    shell_print_colored(str, FONT_COLOR);
}

// Función para imprimir string con color específico
void shell_print_colored(const char* str, uint32_t color) {
    while (*str) {
        if (*str == '\n') {
            shell_newline();
        } else {
            drawChar(*str, color, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
            cursor_x += FONT_SIZE * FONT_BMP_SIZE;
            if (cursor_x >= CHAR_PER_LINE * FONT_SIZE * FONT_BMP_SIZE) {
                shell_newline();
            }
        }
        str++;
    }
}

// Nueva línea
void shell_newline() {
    cursor_x = 0;
    cursor_y += FONT_SIZE * FONT_BMP_SIZE + LINE_Y_PADDING;
    
    // Si llegamos al final de la pantalla, hacer scroll
    if (cursor_y >= SCREEN_HEIGHT - FONT_SIZE) {
        clear_screen();
        cursor_y = 0;
        shell_print_colored("--- Se limpio la pantalla (scroll) ---\n", PROMPT_COLOR);
    }
}

// Mostrar prompt
void shell_print_prompt() {
    shell_print_colored("> ", PROMPT_COLOR);
}

// Limpiar buffer de comandos
void clear_buffer() {
    buffer_pos = 0;
    command_buffer[0] = '\0';
}

// Limpiar pantalla
void clear_screen() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            putPixel(SHELL_COLOR, x, y);
        }
    }
    cursor_x = cursor_y = 0;
}
// Comandos disponibles
void cmd_help() {
    shell_print("Comandos disponibles:\n");
    shell_print("  help     - Mostrar esta ayuda\n");
    shell_print("  clear    - Limpiar pantalla\n");
    shell_print("  echo     - Mostrar texto\n");
    shell_print("\nControles:\n");
    shell_print("  Enter - Ejecutar comando\n");
    shell_print("  Backspace - Borrar carácter\n");
}

void cmd_clear() {
    clear_screen();
}

void cmd_echo(char* args) {
    if (*args) {
        shell_print(args);
    }
   shell_newline();

}

//Altamente experimental, despues la borramos total es un amongus(sus) junto al soporte de unicode
#include "unicodeSupport.h"
void cmd_amongus() {
    // Guardar posición actual del cursor
    uint64_t start_x = cursor_x;
    uint64_t start_y = cursor_y;
    
    // Array con las líneas del Among Us art
    const char* amongus_lines[] = {
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⣤⣤⣀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⠟⠉⠉⠉⠉⠉⠉⠉⠙⠻⢶⣄⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⡏⠀⠀⠀⠀⠀⠀    ⠀⠙⣷⡀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⡟⠀⣠⣶⠛⠛⠛⠛⠛⠛⠳⣦⡀⠀⠘⣿⡄⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠁⠀⢹⣿⣦⣀⣀⣀⣀⣀⣠⣼⡇⠀⠀⠸⣷⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡏⠀⠀⠀⠉⠛⠿⠿⠿⠿⠛⠋⠁⠀⠀⠀⠀⣿",
        "⠀⠀    ⠀⠀⢠⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⡇⠀",
        "      ⠀⠀⣸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⠀⢸⣧⠀",
        "⠀⠀⠀⠀⠀⠀⠀⢸⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⠈⣿⠀",
        "⠀⠀⠀⠀⠀⠀⠀⣾⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⠀⣿⠀",
        "⠀⠀⠀⠀⠀⠀⠀⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀⠀⠀⠀⣿⠀",
        "⠀⠀⠀⠀⠀⠀⢰⣿⠀⠀⠀⠀⣠⡶⠶⠿⠿⠿⠿⢷⣦⠀⠀⠀⠀⠀    ⣿⠀",
        "⠀⠀⣀⣀⣀⠀⣸⡇⠀⠀⠀⠀⣿⡀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀  ⣿⠀",
        "⣠⡿⠛⠛⠛⠛⠻⠀⠀⠀⠀⠀⢸⣇⠀⠀⠀⠀⠀⠀⣿⠇⠀⠀⠀⠀⠀ ⠀⣿⠀",
        "⢻⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⡟⠀⠀⢀⣤⣤⣴⣿⠀⠀⠀⠀⠀⠀ ⠀⣿⠀",
        "⠈⠙⢷⣶⣦⣤⣤⣤⣴⣶⣾⠿⠛⠁⢀⣶⡟⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡟⠀",
        "             ⠀⠈⣿⣆⡀⠀⠀⠀⠀⠀⠀⢀⣠⣴⡾⠃⠀",
        "⠀          ⠀⠀⠀⠀⠈⠛⠻⢿⣿⣾⣿⡿⠿⠟⠋⠁⠀⠀⠀",
        0
    };
    
    // Dibujar cada línea usando la función Unicode
    for (int i = 0; amongus_lines[i] != 0; i++) {
        drawTextUnicode(amongus_lines[i], FONT_COLOR, SHELL_COLOR, 
                       start_x, start_y + i * FONT_SIZE * FONT_BMP_SIZE, FONT_SIZE);
    }
    shell_print(".                             Nachito is sus       		\n");		 
    
    cursor_y = start_y + (sizeof(amongus_lines)/sizeof(amongus_lines[0]) - 1) * FONT_SIZE;
    cursor_x = start_x;
    shell_newline();
}

// Ejecutar comando
void execute_command() {
    if (buffer_pos == 0) return;
    
    command_buffer[buffer_pos] = '\0';
    
    char cmd_copy[BUFFER_SIZE];
    str_copy(cmd_copy, command_buffer);
    char* args = find_args(cmd_copy);
    
    if (str_equals(cmd_copy, "help")) {
        cmd_help();
    } else if (str_equals(cmd_copy, "clear")) {
        cmd_clear();
    } else if (str_equals(cmd_copy, "echo")) {
        cmd_echo(args);
    } else if (str_equals(cmd_copy, "SUS")) {
	cmd_amongus();
    } else if (cmd_copy[0] != '\0') {
        shell_print_colored("Error: ", ERROR_COLOR);
        shell_print("Comando desconocido '");
        shell_print(cmd_copy);
        shell_print("'\n");
        shell_print("Escribe 'help' para ver comandos disponibles.\n");
    }
}
#define LSHIFT_SCANCODE 0x2A
#define RSHIFT_SCANCODE 0x36
    // Manejar entrada del teclado usando syscalls
void handle_keyboard_input() {
    // Actualizar estado de shift usando syscall_isKeyDown
      int shift_pressed = syscall_isKeyDown(LSHIFT_SCANCODE) || syscall_isKeyDown(RSHIFT_SCANCODE); // Left/Right Shift
    // Leer scancode del buffer usando syscall_read
    uint8_t scancode;
    if (syscall_read(0, (char*)&scancode, 1) > 0) {
        // Manejar teclas especiales
        if (scancode == 0x1C) { // Enter
            shell_newline();
            execute_command();
            clear_buffer();
            shell_newline();
            shell_print_prompt();
            return;
        }
        
        if (scancode == 0x0E) { // Backspace
            if (buffer_pos > 0) {
                buffer_pos--;
                command_buffer[buffer_pos] = '\0';
                if (cursor_x >= FONT_SIZE) {
                    cursor_x -= FONT_SIZE;
                    drawChar(' ', FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y, 1);
                }
            }
            return;
        }
        
        // Convertir scancode a ASCII
        char ascii = getAsciiFromMakeCode(scancode, shift_pressed);
        
        // Solo agregar caracteres imprimibles
        if (ascii != 0 && ascii >= 32 && ascii <= 126 && buffer_pos < BUFFER_SIZE - 1) {
            command_buffer[buffer_pos++] = ascii;
            shell_putchar(ascii);
        }
    }
}

// Función principal del shell
void shell_main() {
    clear_screen();
    clear_buffer();
    
    // Mensaje de bienvenida
    shell_print_colored("=======================================\n", PROMPT_COLOR);
    shell_print_colored("    SHELL v14.4\n", PROMPT_COLOR);
    shell_print_colored("=======================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
    
    shell_print_prompt();
    
    // Loop principal
    while (1) {
        handle_keyboard_input();
    }
}

// Punto de entrada principal
int main() {
    shell_main();
    return 0;
}

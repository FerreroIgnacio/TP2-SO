
#include "../standard.h"

#define CHAR_PER_LINE 100
#define FONT_BMP_SIZE 8 ;no se usa actualmente
#define FONT_SIZE 8
#define BUFFER_SIZE 256
#define LINES_PER_SCREEN 75

#define SHELL_COLOR 0x00000F
#define FONT_COLOR 0xFFFFFF
#define ERROR_COLOR 0xFF0000
#define PROMPT_COLOR 0x44DDFF
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Variables globales
static char command_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static int cursor_x = 0;
static int cursor_y = 0;
static int shift_pressed = 0;

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

// Conversión de scancode a ASCII usando la librería
char scan_to_ascii(uint8_t scancode) {
    return getAsciiFromMakeCode(scancode, shift_pressed);
}

// Función para imprimir un carácter
void shell_putchar(char c) {
    if (c == '\n') {
        shell_newline();
        return;
    }
    
    drawChar(c, FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y, 1);
    cursor_x += FONT_SIZE;
    
    if (cursor_x >= CHAR_PER_LINE * FONT_SIZE) {
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
            drawChar(*str, color, SHELL_COLOR, cursor_x, cursor_y, 1);
            cursor_x += FONT_SIZE;
            if (cursor_x >= CHAR_PER_LINE * FONT_SIZE) {
                shell_newline();
            }
        }
        str++;
    }
}

// Nueva línea
void shell_newline() {
    cursor_x = 0;
    cursor_y += FONT_SIZE;
    
    // Si llegamos al final de la pantalla, hacer scroll
    if (cursor_y >= SCREEN_HEIGHT - FONT_SIZE) {
        clear_screen();
        cursor_y = 0;
        shell_print_colored("--- Pantalla limpiada (scroll) ---\n", PROMPT_COLOR);
    }
}

// Mostrar prompt
void shell_print_prompt() {
    shell_print_colored("shell", PROMPT_COLOR);
    shell_print_colored("$ ", FONT_COLOR);
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
/*
// Agregar comando al historial
void add_to_history() {
    if (buffer_pos == 0) return;
    
    // Evitar duplicados consecutivos
    if (history.count > 0 && 
        str_equals(command_buffer, history.commands[(history.count - 1) % MAX_HISTORY])) {
        return;
    }
    
    str_copy(history.commands[history.count % MAX_HISTORY], command_buffer);
    history.count++;
    history.current = history.count;
}

// Navegar por el historial
void navigate_history(int direction) {
    if (history.count == 0) return;
    
    if (direction > 0 && history.current < history.count) {
        history.current++;
    } else if (direction < 0 && history.current > 0) {
        history.current--;
    } else {
        return;
    }
    
    // Borrar línea actual
    int chars_to_clear = buffer_pos + 2; // +2 para "$ "
    for (int i = 0; i < chars_to_clear; i++) {
        if (cursor_x >= FONT_SIZE) {
            cursor_x -= FONT_SIZE;
            drawChar(' ', FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y, 1);
        }
    }
    
    // Mostrar comando del historial
    if (history.current < history.count) {
        int idx = (history.current < MAX_HISTORY) ? history.current : 
                  (history.current % MAX_HISTORY);
        str_copy(command_buffer, history.commands[idx]);
        buffer_pos = str_length(command_buffer);
        shell_print_prompt();
        shell_print(command_buffer);
    } else {
        clear_buffer();
        shell_print_prompt();
    }
}
*/
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
                       start_x, start_y + i * FONT_SIZE, 1);
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
       shift_pressed = syscall_isKeyDown(0x2A) || syscall_isKeyDown(0x36); // Left/Right Shift
 //	 char * msj = syscall_isKeyDown(0x2A) ? "Algun shift down :)" : "Ningun shift down :(";
   //     drawText(msj, 0xFFFFFF, 0x000000, 0, SCREEN_HEIGHT, 3);   
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
    shell_print_colored("    SHELL MEJORADO 14.3\n", PROMPT_COLOR);
    shell_print_colored("=======================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
    
    shell_print_prompt();
    
    // Loop principal
    while (1) {
        handle_keyboard_input();
        // Pequeña pausa para evitar saturar la CPU
        for (int i = 0; i < 1000; i++);
    }
}

// Punto de entrada principal
int main() {
    shell_main();
    return 0;
}

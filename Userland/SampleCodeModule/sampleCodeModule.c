/* sampleCodeModule.c - Shell implementation */

#include "../standard.h"

char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;

#define BUFFER_SIZE 256
#define MAX_ARGS 16

static char command_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;

// Función para leer un carácter usando syscall
char read_char() {
    char c;
    int result = syscall_read(0, &c, 1); // fd=0 (stdin), leer 1 byte
    if (result > 0) {
        return c;
    }
    return 0; // No hay datos disponibles
}
void shell_putchar(char c) {
    drawChar(c, 0xFFFFFF, 0x000000, cursor_x, cursor_y, 1);
    cursor_x += 8;
    if (cursor_x >= 80 * 8) { // Asumiendo 80 caracteres por línea
        cursor_x = 0;
        cursor_y += 8;
    }
}

void shell_print(const char* str) {
    while (*str) {
        shell_putchar(*str);
        str++;
    }
}

void shell_newline() {
    cursor_x = 0;
    cursor_y += 8;
}

void shell_print_prompt() {
    shell_print("$ ");
}

void clear_buffer() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        command_buffer[i] = 0;
    }
    buffer_pos = 0;
}

// Comparar strings
int str_compare(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) return 0;
        str1++;
        str2++;
    }
    return *str1 == *str2;
}

// Comandos del shell
void cmd_help() {
    shell_print("Available commands:\n");
    shell_print("  help    - Show this help message\n");
    shell_print("  clear   - Clear the screen\n");
    shell_print("  echo    - Echo arguments\n");
    shell_print("  test    - Run test function\n");
}

void cmd_clear() {
    // Limpiar pantalla (llenar con negro)
    for (int y = 0; y < 600; y++) {
        for (int x = 0; x < 800; x++) {
            putPixel(0x000000, x, y);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void cmd_echo(char* args) {
    shell_print(args);
}

void cmd_test() {
    extern int testFn();
    int result = testFn();
    shell_print("Test function returned: ");
    drawInt(result, 0xFFFFFF, 0x000000, cursor_x, cursor_y, 1);
    cursor_x += 8 * 8; // Aproximación para el ancho del número
}

// Parsear y ejecutar comando
void execute_command() {
    if (buffer_pos == 0) return;
    
    command_buffer[buffer_pos] = '\0'; // Null terminate
    
    // Encontrar el primer espacio para separar comando de argumentos
    char* args = command_buffer;
    while (*args && *args != ' ') args++;
    if (*args == ' ') {
        *args = '\0';
        args++;
        // Saltar espacios adicionales
        while (*args == ' ') args++;
    }
    
    // Ejecutar comandos
    if (str_compare(command_buffer, "help")) {
        cmd_help();
    } else if (str_compare(command_buffer, "clear")) {
        cmd_clear();
    } else if (str_compare(command_buffer, "echo")) {
        cmd_echo(args);
    } else if (str_compare(command_buffer, "test")) {
        cmd_test();
    } else if (command_buffer[0] != '\0') {
        shell_print("Unknown command: ");
        shell_print(command_buffer);
        shell_print("\nType 'help' for available commands.");
    }
}

// Manejar entrada del teclado
void handle_input(char c) {
    if (c == '\n' || c == '\r') {
        // Enter presionado
        shell_newline();
        execute_command();
        clear_buffer();
        shell_newline();
        shell_print_prompt();
    } else if (c == '\b' || c == 127) {
        // Backspace
        if (buffer_pos > 0) {
            buffer_pos--;
            command_buffer[buffer_pos] = '\0';
            // Retroceder cursor
            if (cursor_x >= 8) {
                cursor_x -= 8;
                drawChar(' ', 0xFFFFFF, 0x000000, cursor_x, cursor_y, 1);
            }
        }
    } else if (c >= 32 && c <= 126) {
        // Carácter imprimible
        if (buffer_pos < BUFFER_SIZE - 1) {
            command_buffer[buffer_pos] = c;
            buffer_pos++;
            shell_putchar(c);
        }
    }
}

// Función principal del shell
void shell_main() {
    // Inicializar
    clear_buffer();
    cursor_x = 0;
    cursor_y = 0;
    
    // Mensaje de bienvenida
    shell_print("Simple Shell v1.0\n");
    shell_print("Type 'help' for available commands.\n\n");
    shell_print_prompt();
    
    // Loop principal
    while (1) {
        char c = read_char();
        if (c != 0) {
            handle_input(c);
        }
        // Aquí podrías agregar halt_cpu() o yield() si tienes multitasking
    }
}

extern int testFn();
int main() {
    shell_main();
    return 0;
}

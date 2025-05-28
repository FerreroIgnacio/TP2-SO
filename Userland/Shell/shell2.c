#include "../standard.h"

#define CHAR_PER_LINE width / (FONT_SIZE * FONT_BMP_SIZE)
#define FONT_BMP_SIZE 8 
#define FONT_SIZE 2
#define BUFFER_SIZE 256
#define LINE_Y_PADDING 4
#define LINES_PER_SCREEN height / ((FONT_SIZE * FONT_BMP_SIZE) + LINE_Y_PADDING)

#define SHELL_COLOR 0x00000A
#define FONT_COLOR 0xAAAAAA
#define ERROR_COLOR 0xAA4444
#define PROMPT_COLOR 0x44AAA4

#define KEYS_PER_LOOP 7

// Variables globales
uint8_t * fb;
uint16_t width=0, height=0, bpp=0, pitch=0;
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
void hide_cursor();
void execute_command();

// Comandos disponibles
void cmd_help();
void cmd_clear();
void cmd_echo(char* args);
void cmd_test();
void cmd_info();
void cmd_dateTime();

// Utilidades de string
int str_equals(const char* s1, const char* s2);
int str_length(const char* str);
char* find_args(char* cmd);
void str_copy(char* dest, const char* src);


//programas
static void * const pongisgolfModuleAddress = (void*)0x1000000;

typedef int (*EntryPoint)();



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
    //drawChar(cursor_x, cursor_y, c, FONT_COLOR);
    fbDrawChar(fb, c, FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
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
            //drawChar(cursor_x, cursor_y, *str, color);
	        fbDrawChar(fb, *str, color, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
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
    hide_cursor();
	cursor_x = 0;
    cursor_y += FONT_SIZE * FONT_BMP_SIZE + LINE_Y_PADDING;
    
    // Si llegamos al final de la pantalla, hacer scroll
    if (cursor_y >= height - FONT_SIZE) {
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
	fbFill (fb, SHELL_COLOR);
    cursor_x = cursor_y = 0;
}
// Comandos disponibles
void cmd_help() {
    shell_print("Comandos disponibles:\n");
    shell_print("  help      - Mostrar esta ayuda\n");
    shell_print("  clear     - Limpiar pantalla\n");
    shell_print("  echo      - Mostrar texto\n");
    shell_print("  datetime  - Mostrar fecha y hora\n");
    shell_print("\nProgramas disponibles:\n");
    shell_print("  pongisgolf\n");
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

void cmd_dateTime(){
    uint8_t year=0,month=0,day=0,hours=0,minutes=0,seconds=0;
    char str [18];

    getLocalTime(&hours,&minutes,&seconds);
    getLocalDate(&year,&month,&day);

    itos_padded(day, str,2);
    itos_padded(month, str+3,2);
    itos_padded(year, str+6,2);
    itos_padded(hours, str+9,2);
    itos_padded(minutes, str+12,2);
    itos_padded(seconds, str+15,2);
    str[2]=str[5]='-';
    str[8]=' ';
    str[11]=str[14]=':';
    str[17] = 0;

    shell_print(str);
    shell_newline();
}


void cmd_fps(){
    char str [15] = "FPS: ";
    itos_padded(getFps(), str+4,5);
    shell_print(str);
    shell_newline();
}




// Ejecutar comando
void execute_command() {
    hide_cursor(); 
    if (buffer_pos == 0) return;
    // Borro el cursor antes de ejecutar comando
    //drawChar(cursor_x, cursor_y, ' ', PROMPT_COLOR);
    fbDrawChar(fb,' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);

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
    } else if (str_equals(cmd_copy, "datetime")) {
	    cmd_dateTime();
    } else if (str_equals(cmd_copy, "fps")) {
	    cmd_fps();
    } else if (str_equals(cmd_copy, "pongisgolf")) {
	    ((EntryPoint)pongisgolfModuleAddress)();    
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

static int cursor_visible = 1;
static int last_cursor_time = 0;
static int cursor_drawn = 0; 
#define CURSOR_BLINK_INTERVAL 80  //en ticks

void update_cursor() {
    int current_time = getBootTime();
    int should_blink = (current_time - last_cursor_time >= CURSOR_BLINK_INTERVAL);
    
    if (should_blink) {
        cursor_visible = !cursor_visible;
        last_cursor_time = current_time;
    }
    
    // Solo actualizar la pantalla si el estado visual cambió
    int should_be_drawn = cursor_visible;
    if (cursor_drawn != should_be_drawn) {
        char cursor_char = should_be_drawn ? '_' : ' ';
        //drawChar(cursor_x, cursor_y, cursor_char, PROMPT_COLOR);
	    fbDrawChar(fb, cursor_char, PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
        cursor_drawn = should_be_drawn;
    }
}

// Función para forzar cursor visible (llamar después de escribir)
void reset_cursor() {
    cursor_visible = 1;
    last_cursor_time = getBootTime();
    if (!cursor_drawn) {
        //drawChar(cursor_x, cursor_y, '_', PROMPT_COLOR);
	    fbDrawChar(fb, '_', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
        cursor_drawn = 1;
    }
}

// Función para ocultar cursor (llamar antes de escribir texto)
void hide_cursor() {
    if (cursor_drawn) {
        //drawChar(cursor_x, cursor_y, ' ', SHELL_COLOR);
	    fbDrawChar(fb, ' ', SHELL_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
        cursor_drawn = 0;
    }
}

#define LSHIFT_SCANCODE 0x2A
#define RSHIFT_SCANCODE 0x36
    // Manejar entrada del teclado usando syscalls
void handle_keyboard_input() {
	update_cursor();  

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
		        hide_cursor();
                command_buffer[buffer_pos] = '\0';
                if (cursor_x >= FONT_SIZE * FONT_BMP_SIZE) {
                    cursor_x -= FONT_SIZE * FONT_BMP_SIZE;
                 	//drawChar(cursor_x, cursor_y, ' ', FONT_COLOR);
		            fbDrawChar(fb, ' ', FONT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
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
    fpsInit();
    clear_screen();
    clear_buffer();
    
    // Mensaje de bienvenida
    shell_print_colored("=======================================\n", PROMPT_COLOR);
    shell_print_colored("    SHELL v15.1\n", PROMPT_COLOR);
    shell_print_colored("=======================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
    
    shell_print_prompt();
    
    // Loop principal
    while (1) {
        for (int i = 0 ; i < KEYS_PER_LOOP ; i++)
            handle_keyboard_input();
        fbSet(fb);
        incFramesCount();
    }
}


// Punto de entrada principal
uint8_t newFb [100000000]; // CAMBIAR POR MALLOC
int main() {
    fb = newFb;
    getVideoData(&width,&height,&bpp,&pitch);
    shell_main();
    return 0;
}
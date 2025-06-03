#include "../standard.h"

#define CHAR_PER_LINE width / (FONT_SIZE * FONT_BMP_SIZE)
#define FONT_BMP_SIZE 8 
#define FONT_SIZE 2
#define BUFFER_SIZE 256
#define LINE_Y_PADDING 4
#define LINES_PER_SCREEN height / ((FONT_SIZE * FONT_BMP_SIZE) + LINE_Y_PADDING)
#define STDOUT_BUFFER_SIZE 4096
#define FRAMEBUFFER_SIZE 100000000

#define SHELL_COLOR 0x00000A
#define FONT_COLOR 0xAAAAAA
#define ERROR_COLOR 0xAA4444
#define PROMPT_COLOR 0x44AAA4

#define KEYS_PER_LOOP 8

#define LSHIFT_MAKECODE 0x2A
#define RSHIFT_MAKECODE 0x36
#define LCTRL_MAKECODE  0x1D
#define LALT_MAKECODE   0x38
#define TAB_MAKECODE    0x0F
#define ESC_MAKECODE    0x01


// Variables globales
uint8_t * fb;
uint16_t width=0, height=0, bpp=0, pitch=0;
static char command_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static int cursor_x = 0;
static int cursor_y = 0;
static char firstEntry = 1;

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
void cmd_fps();
void cmd_registers();

// Utilidades de string
char* find_args(char* cmd);


//programas
static void * const pongisgolfModuleAddress = (void*)0x600000;

typedef int (*EntryPoint)();

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
    shell_print("  help      - Mostrar comandos disponibles\n");
    shell_print("  clear     - Limpiar pantalla\n");
    shell_print("  echo      - Mostrar texto\n");
    shell_print("  datetime  - Mostrar fecha y hora UTC-0 \n");
    shell_print("  registers - Imprimir registros guardados (Ctrl+R)\n");
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

void cmd_dateTime(){
    uint8_t year=0,month=0,day=0,hours=0,minutes=0,seconds=0;
    getLocalTime(&hours,&minutes,&seconds);
    getLocalDate(&year,&month,&day);
    printf("Fecha y hora en UTC-0: %d/%d/20%d %d:%d:%d\n",day, month, year, hours, minutes, seconds);
}


void cmd_fps(){
    printf("FPS: %d", getFps());
}

void cmd_registers(){
    registers_t regs;
    getRegisters(&regs);
    printf("RIP: %#P\n"
        "RFLAGS: %#P\n"
        "RSP: %#P\n"
        "RBP: %#P\n"
        "RAX: %#P\n"
        "RBX: %#P\n"
        "RCX: %#P\n"
        "RDX: %#P\n"
        "RSI: %#P\n"
        "RDI: %#P\n"
        "R8 : %#P\n"
        "R9 : %#P\n"
        "R10: %#P\n"
        "R11: %#P\n"
        "R12: %#P\n"
        "R13: %#P\n"
        "R14: %#P\n"
        "R15: %#P\n",
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
    hide_cursor(); 
    if (buffer_pos == 0) return;

    // Borro el cursor antes de ejecutar comando
    fbDrawChar(fb,' ', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);

    command_buffer[buffer_pos] = '\0';
    
    char cmd_copy[BUFFER_SIZE];
    strcpy(cmd_copy, command_buffer);
    char* args = find_args(cmd_copy);
    
    if (!strcmp(cmd_copy, "help")) {
        cmd_help();
    } else if (!strcmp(cmd_copy, "clear")) {
        cmd_clear();
    } else if (!strcmp(cmd_copy, "echo")) {
        cmd_echo(args);
    } else if (!strcmp(cmd_copy, "datetime")) {
	    cmd_dateTime();
    } else if (!strcmp(cmd_copy, "fps")) {
	    cmd_fps();
    } else if (!strcmp(cmd_copy, "registers")) {
	    cmd_registers();
    } else if (!strcmp(cmd_copy, "pongisgolf")) {
	    ((EntryPoint)pongisgolfModuleAddress)();    
    } else if (!strcmp(cmd_copy, "SUS")) {
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
	    fbDrawChar(fb, cursor_char, PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
        cursor_drawn = should_be_drawn;
    }
}

// Función para forzar cursor visible (llamar después de escribir)
void reset_cursor() {
    cursor_visible = 1;
    last_cursor_time = getBootTime();
    if (!cursor_drawn) {
	    fbDrawChar(fb, '_', PROMPT_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
        cursor_drawn = 1;
    }
}

// Función para ocultar cursor (llamar antes de escribir texto)
void hide_cursor() {
    if (cursor_drawn) {
	    fbDrawChar(fb, ' ', SHELL_COLOR, SHELL_COLOR, cursor_x, cursor_y, FONT_SIZE);
        cursor_drawn = 0;
    }
}


    // Manejar entrada del teclado usando syscalls
void handle_keyboard_input() {
	update_cursor();  

    // Actualizar estado de shift usando isKeyPressed
      int shift_pressed = isKeyPressed(LSHIFT_MAKECODE) || isKeyPressed(RSHIFT_MAKECODE); // Left/Right Shift
    // Leer makecode del buffer usando read
    uint8_t makecode;
    if (read(0, (char*)&makecode, 1) > 0) {
        
        if (makecode == 0xE0){
            read(0, (char*)&makecode, 1);
            return;
        }
        // Manejar teclas especiales
        if (makecode == 0x1C) { // Enter
            shell_newline();
            execute_command();
	        clear_buffer();
	        shell_newline();
            shell_print_prompt();
            return;
        }
        
        if (makecode == 0x0E) { // Backspace
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
        
        // Convertir makecode a ASCII
        char ascii = getAsciiFromMakeCode(makecode, shift_pressed);
        
        // Solo agregar caracteres imprimibles
        if (ascii != 0 && ascii >= 32 && ascii <= 126 && buffer_pos < BUFFER_SIZE - 1) {
            command_buffer[buffer_pos++] = ascii;
            shell_putchar(ascii);
        }
    }
}

void shell_welcome(){
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print_colored("             SHELL v16.0\n", PROMPT_COLOR);
    shell_print_colored("=================================================\n", PROMPT_COLOR);
    shell_print("Escribe 'help' para ver comandos disponibles.\n\n");
}

// Punto de entrada principal
uint8_t newFb [FRAMEBUFFER_SIZE]; // CAMBIAR POR MALLOC
int main() {

    if (firstEntry){
        fb = newFb;
        getVideoData(&width,&height,&bpp,&pitch);
        fpsInit();
        clear_screen();
        clear_buffer();
        shell_welcome();
        shell_print_prompt();
        firstEntry = 0;
    }

    while (1) {
        char stdoutBuff [STDOUT_BUFFER_SIZE];
        read(STDOUT,stdoutBuff,STDOUT_BUFFER_SIZE);
        if (strlen(stdoutBuff)>0){
            cursor_x = 0;
            shell_print(stdoutBuff);
            shell_newline();
            shell_print_prompt();
        }

        for (int i = 0 ; i < KEYS_PER_LOOP ; i++)
            handle_keyboard_input();

        fbSet(fb);
    }
    
    shell_main();
    return 0;
}

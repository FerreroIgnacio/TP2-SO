#include <shell.h>

#define MAXLENGTH 4098

const unsigned char scan_code_to_ascii_es[] = {
    /* 0x00 */ 0,    // No key
    /* 0x01 */ 0,    // F9 (No ASCII)
    /* 0x02 */ 0,    // (Reserved)
    /* 0x03 */ 0,    // F5 (No ASCII)
    /* 0x04 */ 'a',  // a
    /* 0x05 */ 'b',  // b
    /* 0x06 */ 'c',  // c
    /* 0x07 */ 'd',  // d
    /* 0x08 */ 'e',  // e
    /* 0x09 */ 'f',  // f
    /* 0x0A */ 'g',  // g
    /* 0x0B */ 'h',  // h
    /* 0x0C */ 'i',  // i
    /* 0x0D */ 'j',  // j
    /* 0x0E */ 'k',  // k
    /* 0x0F */ 'l',  // l
    /* 0x10 */ 'm',  // m
    /* 0x11 */ 'n',  // n
    /* 0x12 */ 'ñ',  // ñ (Código ISO 164)
    /* 0x13 */ 'o',  // o
    /* 0x14 */ 'p',  // p
    /* 0x15 */ 'q',  // q
    /* 0x16 */ 'r',  // r
    /* 0x17 */ 's',  // s
    /* 0x18 */ 't',  // t
    /* 0x19 */ 'u',  // u
    /* 0x1A */ 'v',  // v
    /* 0x1B */ 'w',  // w
    /* 0x1C */ 'x',  // x
    /* 0x1D */ 'y',  // y
    /* 0x1E */ 'z',  // z
    /* 0x1F */ '1',  // 1
    /* 0x20 */ '2',  // 2
    /* 0x21 */ '3',  // 3
    /* 0x22 */ '4',  // 4
    /* 0x23 */ '5',  // 5
    /* 0x24 */ '6',  // 6
    /* 0x25 */ '7',  // 7
    /* 0x26 */ '8',  // 8
    /* 0x27 */ '9',  // 9
    /* 0x28 */ '0',  // 0
    /* 0x29 */ '\'', // '
    /* 0x2A */ '¡',  // ¡ (Shift+1)
    /* 0x2B */ '´',  // ´ (dead key)
    /* 0x2C */ 'ç',  // ç (ISO 135)
    /* 0x2D */ '+',  // +
    /* 0x2E */ '`',  // ` (dead key)
    /* 0x2F */ 'º',  // º
    /* 0x30 */ 'ª',  // ª
    /* 0x31 */ '\\', // Backslash (ISO 94)
    /* 0x32 */ '<',  // <
    /* 0x33 */ ',',  // ,
    /* 0x34 */ '.',  // .
    /* 0x35 */ '-',  // -
    /* 0x36 */ 0,    // (CapsLock, no ASCII)
    /* 0x37 */ 0,    // (F2, no ASCII)
    /* 0x38 */ 0,    // (Left Shift, no ASCII)
    /* 0x39 */ 0,    // (ISO Key, no ASCII)
    /* 0x3A */ '´',  // ´ (dead key, pressed alone)
    /* 0x3B */ ';',  // ;
    /* 0x3C */ '´',  // ´ (dead key variant)
    /* 0x3D */ 0,    // (F3, no ASCII)
    /* 0x3E */ 0,    // (F4, no ASCII)
    /* 0x3F */ 0     // (F7, no ASCII)
    // ... (continuar según necesidad)
};

#define FONTSIZE 1
#define Y_PADDING 5
#define X_PADDING 5
#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define SHELLCOLOR 0x000022
char buffer[MAXLENGTH] = {0};
int index = 0;

int cursorX = X_PADDING * 2;
int cursorY = 0;

void shellKeyboardHandler(uint8_t scancode){
	//no manejar cuando sueltan
	if(scancode == 0)
		scancode = '_';
	if(scancode & 0x80)
		return;
	buffer[index] = scan_code_to_ascii_es[scancode & 0x7F];
	putText(buffer, WHITE, SHELLCOLOR, X_PADDING * 2 + (8 * FONTSIZE /*skipear el primer >*/), cursorY, FONTSIZE); 	
	index++;
}


void start_shell(void) {
	setKeyboardHandler(shellKeyboardHandler);
	fillScreen(0x000055);
	putChar('>', 0x000000, 0x000055, X_PADDING, Y_PADDING, FONTSIZE); 
	
}

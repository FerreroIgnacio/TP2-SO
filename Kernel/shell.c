#include <shell.h>
#include <util.h>
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
#define SHELLCOLOR 0x00000A
#define FONTCOLOR 0xFFFFFF
			//padding + '>' * fontsize
#define WRITESTART X_PADDING * 2 + 8 * FONTSIZE
char buffer[MAXLENGTH] = {0};
int index = 0;

int cursorX = X_PADDING * 2;
static int cursorY = Y_PADDING;

#include <stdint.h>

// Solo símbolos explícitos
char validSymbols[256] = {
    ['('] = 1,
    [')'] = 1,
    ['{'] = 1,
    ['}'] = 1,
    ['['] = 1,
    [']'] = 1,
    ['|'] = 1,
    ['&'] = 1,
    [';'] = 1,
    ['<'] = 1,
    ['>'] = 1,
    ['!'] = 1,
    ['='] = 1,
    ['+'] = 1,
    ['-'] = 1,
    ['*'] = 1,
    ['/'] = 1,
    ['%'] = 1,
    [':'] = 1,
    ['_'] = 1,
    ['.'] = 1,
    [','] = 1,
    ['?'] = 1,
    ['\''] = 1,
    ['"'] = 1,
    ['#'] = 1,
    ['\\'] = 1,
    ['`'] = 1
};

// Verifica si c es símbolo válido: letra, dígito, espacio o símbolo especial
int isValidSymbol(char c) {
    // Letras mayúsculas o minúsculas
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return 1;

    // Dígitos
    if (c >= '0' && c <= '9')
        return 1;

    // Espacio
    if (c == ' ')
        return 1;

    // Símbolos especiales en arr
    return validSymbols[(uint8_t)c];
}

void shellKeyboardHandler(uint8_t scancode) {
    if (scancode & 0x80) // Ignorar key releases
        return;

    int ascii = scan_code_to_ascii_es[scancode];
    
    //int index = 0;
    
    if (isValidSymbol(ascii) ) {
        if (index < MAXLENGTH) {
            buffer[index++] = (char)ascii;
        }
    } else {
        // Formato <NUM> directamente en el buffer
        if (index + 3 < MAXLENGTH) { // Espacio para <XX>
            buffer[index++] = '<';
            
            char numBuf[12] = {0};
            char *str = stringFromInt(ascii, numBuf);
            
            // Copiar dígitos
            for (int i = 0; str[i] != 0 && index < MAXLENGTH; i++) {
                buffer[index++] = str[i];
            }
            
            if (index < MAXLENGTH) {
                buffer[index++] = '>';
            }
        }
    }
	putText(buffer, WHITE, SHELLCOLOR, X_PADDING * 2 + 8 * FONTSIZE, cursorY, FONTSIZE);
    	
	int i = 0;
	while(buffer[i] != 0){
		if(getWidth() - X_PADDING < i * 8 * FONTSIZE){
			cursorX = 0;
			char buf[256] = {0};
			cursorY += 8 * FONTSIZE;
			putText(stringFromInt(cursorY, buf), 0xFF0000, 0xFFFFFF, 0, 0, 4);
		}
		putChar(buffer[i], FONTCOLOR, SHELLCOLOR, WRITESTART + cursorX, cursorY, FONTSIZE);

	}
}
void start_shell(void) {
	setKeyboardHandler(shellKeyboardHandler);
	fillScreen(SHELLCOLOR);
	putChar('>', FONTCOLOR, SHELLCOLOR, X_PADDING, Y_PADDING, FONTSIZE); 
	
}

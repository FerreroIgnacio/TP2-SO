#include "standard.h"

#include "font8x8/font8x8_basic.h"
//uint16_t getWidth();
//uint16_t getHeight();

// Reemplaza las tablas en standard.c con estas versiones corregidas para layout argentino

// Make code to ASCII mapping for Set 1 scancodes - Layout Argentino
char makeCodeToAscii[128] = {
    0,      // 0x00 - unused
    0,      // 0x01 - ESC
    '1',    // 0x02
    '2',    // 0x03
    '3',    // 0x04
    '4',    // 0x05
    '5',    // 0x06
    '6',    // 0x07
    '7',    // 0x08
    '8',    // 0x09
    '9',    // 0x0A
    '0',    // 0x0B
    '\'',   // 0x0C - apostrofe (en lugar de -)
    '¿',    // 0x0D - ¿ (en lugar de =)
    0,      // 0x0E - Backspace
    0,      // 0x0F - Tab
    'q',    // 0x10
    'w',    // 0x11
    'e',    // 0x12
    'r',    // 0x13
    't',    // 0x14
    'y',    // 0x15
    'u',    // 0x16
    'i',    // 0x17
    'o',    // 0x18
    'p',    // 0x19
    0,      // 0x1A - ´ (dead key, puede ser problemático)
    '+',    // 0x1B - +
    0,      // 0x1C - Enter
    0,      // 0x1D - Left Ctrl
    'a',    // 0x1E
    's',    // 0x1F
    'd',    // 0x20
    'f',    // 0x21
    'g',    // 0x22
    'h',    // 0x23
    'j',    // 0x24
    'k',    // 0x25
    'l',    // 0x26
    'ñ',    // 0x27 - ñ
    '{',    // 0x28 - { (en algunos layouts argentinos)
    '|',    // 0x29 - |
    0,      // 0x2A - Left Shift
    '}',    // 0x2B - }
    'z',    // 0x2C
    'x',    // 0x2D
    'c',    // 0x2E
    'v',    // 0x2F
    'b',    // 0x30
    'n',    // 0x31
    'm',    // 0x32
    ',',    // 0x33
    '.',    // 0x34
    '-',    // 0x35 - - (guión)
    0,      // 0x36 - Right Shift
    '*',    // 0x37 - Keypad *
    0,      // 0x38 - Left Alt
    ' ',    // 0x39 - Space
    0,      // 0x3A - Caps Lock
    0,      // 0x3B - F1
    0,      // 0x3C - F2
    0,      // 0x3D - F3
    0,      // 0x3E - F4
    0,      // 0x3F - F5
    0,      // 0x40 - F6
    0,      // 0x41 - F7
    0,      // 0x42 - F8
    0,      // 0x43 - F9
    0,      // 0x44 - F10
    0,      // 0x45 - Num Lock
    0,      // 0x46 - Scroll Lock
    '7',    // 0x47 - Keypad 7
    '8',    // 0x48 - Keypad 8
    '9',    // 0x49 - Keypad 9
    '-',    // 0x4A - Keypad -
    '4',    // 0x4B - Keypad 4
    '5',    // 0x4C - Keypad 5
    '6',    // 0x4D - Keypad 6
    '+',    // 0x4E - Keypad +
    '1',    // 0x4F - Keypad 1
    '2',    // 0x50 - Keypad 2
    '3',    // 0x51 - Keypad 3
    '0',    // 0x52 - Keypad 0
    '.',    // 0x53 - Keypad .
     0,    // 0x54
     0,  // 0x55
     '<',   // 0x56 '<'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x54-0x5D unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x5E-0x67 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x68-0x71 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x72-0x7B unused
    0, 0, 0, 0                     // 0x7C-0x7F unused
};

// Shifted characters mapping - Layout Argentino
char makeCodeToAsciiShifted[128] = {

    0,      // 0x00 - unused
    0,      // 0x01 - ESC
    '!',    // 0x02 - Shift+1
    '"',    // 0x03 - Shift+2
    '#',    // 0x04 - Shift+3
    '$',    // 0x05 - Shift+4
    '%',    // 0x06 - Shift+5
    '&',    // 0x07 - Shift+6
    '/',    // 0x08 - Shift+7
    '(',    // 0x09 - Shift+8
    ')',    // 0x0A - Shift+9
    '=',    // 0x0B - Shift+0
    '?',    // 0x0C - Shift+' (apostrofe)
    '¡',    // 0x0D - Shift+¿
    0,      // 0x0E - Backspace
    0,      // 0x0F - Tab
    'Q',    // 0x10
    'W',    // 0x11
    'E',    // 0x12
    'R',    // 0x13
    'T',    // 0x14
    'Y',    // 0x15
    'U',    // 0x16
    'I',    // 0x17
    'O',    // 0x18
    'P',    // 0x19
    0,      // 0x1A - Shift+´ (dead key)
    '*',    // 0x1B - Shift++
    0,      // 0x1C - Enter
    0,      // 0x1D - Left Ctrl
    'A',    // 0x1E
    'S',    // 0x1F
    'D',    // 0x20
    'F',    // 0x21
    'G',    // 0x22
    'H',    // 0x23
    'J',    // 0x24
    'K',    // 0x25
    'L',    // 0x26
    'Ñ',    // 0x27 - Shift+ñ
    '[',    // 0x28 - Shift+{
    '°',    // 0x29 - Shift+|
    0,      // 0x2A - Left Shift
    ']',    // 0x2B - Shift+}
    'Z',    // 0x2C
    'X',    // 0x2D
    'C',    // 0x2E
    'V',    // 0x2F
    'B',    // 0x30
    'N',    // 0x31
    'M',    // 0x32
    ';',    // 0x33 - Shift+,
    ':',    // 0x34 - Shift+.
    '_',    // 0x35 - Shift+-
    0,      // 0x36 - Right Shift
    '*',    // 0x37 - Keypad *
    0,      // 0x38 - Left Alt
    ' ',    // 0x39 - Space
    0,      // 0x3A - Caps Lock
    0,      // 0x3B - F1
    0,      // 0x3C - F2
    0,      // 0x3D - F3
    0,      // 0x3E - F4
    0,      // 0x3F - F5
    0,      // 0x40 - F6
    0,      // 0x41 - F7
    0,      // 0x42 - F8
    0,      // 0x43 - F9
    0,      // 0x44 - F10
    0,      // 0x45 - Num Lock
    0,      // 0x46 - Scroll Lock
    '7',    // 0x47 - Keypad 7
    '8',    // 0x48 - Keypad 8
    '9',    // 0x49 - Keypad 9
    '-',    // 0x4A - Keypad -
    '4',    // 0x4B - Keypad 4
    '5',    // 0x4C - Keypad 5
    '6',    // 0x4D - Keypad 6
    '+',    // 0x4E - Keypad +
    '1',    // 0x4F - Keypad 1
    '2',    // 0x50 - Keypad 2
    '3',    // 0x51 - Keypad 3
    '0',    // 0x52 - Keypad 0
    '.',    // 0x53 - Keypad .i
      0,    // 0x54
     0,  // 0x55
     '>',    // 0x56 '>'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x54-0x5D unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x5E-0x67 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x68-0x71 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x72-0x7B unused
    0, 0, 0, 0                     // 0x7C-0x7F unused
};
char getAsciiFromMakeCode(uint8_t makeCode, int shifted) {
    if (makeCode >= 128) return 0;
    return shifted ? makeCodeToAsciiShifted[makeCode] : makeCodeToAscii[makeCode];
}

// Escribe el char str en la posición (x,y)
 void drawChar(char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
    if(ascii < 0 || ascii > 128) 
	    return;
	char * bmp = font8x8_basic[ascii];

    for(int j = 0; j < 64; j++){
        int fil = j / 8;    // fila (0-7)
        int col = j % 8;    // columna (0-7)

        // Acceder al byte de la fila y verificar el bit de la columna
        int isOn = bmp[fil] & (1 << col);  // Sin invertir el bit
        int color = isOn ? hexColor : backColor;

        for (int dx = 0; dx < size; dx++) {
            for (int dy = 0; dy < size; dy++) {
                putPixel(color,
                        x + col * size + dx,     // X: posición + columna
                        y + fil * size + dy);    // Y: posición + fila
            }
        }
    }
 }

// Escribe el string str en la posición (x,y)
void drawText(char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
	//para cada letra
	int i = 0;
	while(str[i] != 0){
		drawChar(str[i], hexColor, backColor, x, y, size);
		x+= 8 * size;
		i++;
	}
}

// Dibuja un número entero en la pantalla en la posición (x,y)
void drawInt(int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size) {
    char buffer[12];
    int i = 0;
    
    // Manejo numeros negativos
    if (num < 0) {
        buffer[i++] = '-';
        num = -num;
    }
    
    // Caso especial: 0
    if (num == 0) {
        buffer[i++] = '0';
    } else {
        // Extraer dígitos en orden inverso
        int start = i;
        while (num > 0) {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        }
        // Invertir solo los dígitos
        for (int j = start; j < (start + i) / 2; j++) {
            char temp = buffer[j];
            buffer[j] = buffer[i - 1 - (j - start)];
            buffer[i - 1 - (j - start)] = temp;
        }
    }
    
    buffer[i] = '\0';
    drawText(buffer, hexColor, backColor, x, y, size);
}


// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
// Siendo x,y la esquina inferior del rectángulo
//void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
//void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t r);

// Cambia todos los píxeles a hexColor
//void fillScreen(uint32_t hexColor);


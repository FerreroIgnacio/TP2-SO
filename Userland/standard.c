#include "standard.h"
#include "font8x8/font8x8_basic.h"

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
    191,    // 0x0D - ¿ (en lugar de =)
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
    241,    // 0x27 - ñ
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
    0                              // 0x7C-0x7F unused
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
    0,      // 0x0D - Shift+¿                   ARREGLAR ¡
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
    0,      // 0x27 - Shift+ñ                   ARREGLAR Ñ
    '[',    // 0x28 - Shift+{
    0,      // 0x29 - Shift+|                    ARREGLAR °
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
    0,                             // 0x7C-0x7F unused
};

char getAsciiFromMakeCode(uint8_t makeCode, int shifted) {
    if (makeCode >= 128) return 0;
    return shifted ? makeCodeToAsciiShifted[makeCode] : makeCodeToAscii[makeCode];
}


uint64_t fbGetSize (){
    uint16_t height, pitch;
    getVideoData(0, &height, 0, &pitch);
    return pitch * height;
}

void fbPutPixel(uint8_t * fb, uint32_t hexColor, uint64_t x, uint64_t y, uint64_t bpp, uint64_t pitch) {
    uint64_t offset = (x * (bpp/8)) + (y * pitch);
    fb[offset]     =  (hexColor) & 0xFF;
    fb[offset+1]   =  (hexColor >> 8) & 0xFF; 
    fb[offset+2]   =  (hexColor >> 16) & 0xFF;
}

void fbDrawChar(uint8_t * fb, char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
    
    if(ascii < 0 || ascii > 128) 
	    return;
	char * bmp = font8x8_basic[(unsigned char)ascii];

    uint16_t bpp, pitch;
    getVideoData(0, 0, &bpp, &pitch);   

    for(int j = 0; j < 64; j++){
        int fil = j / 8;    // fila (0-7)
        int col = j % 8;    // columna (0-7)

        // Acceder al byte de la fila y verificar el bit de la columna
        //int isOn = bmp[fil] & (1 << col);  // Sin invertir el bit
        int color = (bmp[fil] & (1 << col)) ? hexColor : backColor;
        
    
        for (int dx = 0; dx < size; dx++) {
            for (int dy = 0; dy < size; dy++) {
                fbPutPixel(fb, color, 
                        x + col * size + dx,     // X: posición + columna
                        y + fil * size + dy,    // Y: posición + fila
                        bpp, pitch);
            }
        }
    }
}

void fbDrawText(uint8_t * fb, char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
	int i = 0;
	while(str[i] != 0){
		fbDrawChar(fb, str[i], hexColor, backColor, x, y, size);
		x+= 8 * size;
		i++;
	}
}

void fbDrawInt(uint8_t * fb, int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
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
    fbDrawText(fb, buffer, hexColor, backColor, x, y, size);
}




void fbFill(uint8_t * fb, uint32_t hexColor){
    uint16_t bpp, pitch, width, height;
    getVideoData(&width, &height, &bpp, &pitch);
    uint8_t r = (hexColor >> 16) & 0xFF;
    uint8_t g = (hexColor >> 8) & 0xFF;
    uint8_t b = hexColor & 0xFF;
    uint8_t bytesPerPixel = bpp / 8;
    
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint64_t offset = y * pitch + x * bytesPerPixel;
            fb[offset] = b;
            fb[offset + 1] = g;
            fb[offset + 2] = r;
        }
    }
}

uint64_t framesCount, timerCount;
void incFramesCount(){
    framesCount++;
}
void fpsInit(){
    timerCount = getBootTime();
    framesCount = 0;
}

//retorna la cantidad de frames 
uint64_t getFps(){
    uint64_t time = getBootTime()-timerCount;
    if (time < 100) return 0;
    uint64_t fps  = (1000 * framesCount)/time;
    fpsInit();
    return fps;
};

void itos(uint64_t value, char* str) {
    // Caso especial: si el valor es 0
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // Buffer temporal para construir el string al revés
    char temp[21]; // máximo 20 dígitos para uint64_t + null terminator
    int index = 0;
    
    // Extraer dígitos (quedan en orden inverso)
    while (value > 0) {
        temp[index] = (value % 10) + '0';
        value /= 10;
        index++;
    }
    
    // Copiar al string destino en orden correcto
    int i;
    for (i = 0; i < index; i++) {
        str[i] = temp[index - 1 - i];
    }
    str[i] = '\0'; // null terminator
}

void itos_padded(uint64_t value, char* str, int width) {

    char temp[21];
    itos(value, temp);
    
    int len = 0;
    while (temp[len] != '\0') len++;
    
    int padding = width - len;
    int i;
    for (i = 0; i < padding; i++) {
        str[i] = '0';
    }
    
    for (i = 0; i < len; i++) {
        str[padding + i] = temp[i];
    }
    
    // no agrega null
}


/* Funciones mejoradas con fbSetRegion B) */
/* SUS ඞ */

/*
 * dado que el formato es RGB no RGBA este valor no se usa nunca
 * lo podemos utilizar cuando no quiero utilizar la mascara
 * Si quisieramos expandir a RGBA o algun formato con mayor bpp
 * basta con cambiar el tipo de los parametros de color y setear NO_COLOR_MASK a un valor fuera del rango del bpp
 */
#define NO_COLOR_MASK 0xFFFFFF + 1
#define BPP 3
#define TEMP_ALLOC_LEN 500 //despues reemplazamos por malloc(width, height);

/*
 * Dado dos colores genera un color de mascara diferente a ellos dos
 */
uint32_t generateMaskColor(uint32_t colorA, uint32_t colorB){
	if(colorA + 1 == colorB){
		return colorB + 1;
	}
	return colorA + 1;
}
			
void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    uint8_t bmp[BPP];
    bmp[0] = color & 0xFF;
    bmp[1] = (color >> 8) & 0xFF;
    bmp[2] = (color >> 16) & 0xFF;
    fbSetRegion(x, y, 1, 1, bmp, color + 1 /* no mask */);
}
void drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    uint8_t bmp[TEMP_ALLOC_LEN][TEMP_ALLOC_LEN][BPP];
    uint32_t maskColor = color + 1;
    for (uint32_t j = 0; j < height; j++)
        for (uint32_t i = 0; i < width; i++) {
            bmp[j][i][0] = color & 0xFF;
            bmp[j][i][1] = (color >> 8) & 0xFF;
            bmp[j][i][2] = (color >> 16) & 0xFF;
        }
    fbSetRegion(x, y, width, height, (uint8_t*)bmp, maskColor); // sin máscara
}
void drawCircle(uint32_t x, uint32_t y, uint32_t r, uint32_t color) {
    uint32_t d = r * 2 + 1;
    uint8_t bmp[TEMP_ALLOC_LEN][TEMP_ALLOC_LEN][3];
    uint32_t maskColor = color + 1;
    for (uint32_t j = 0; j < d; j++) {
        for (uint32_t i = 0; i < d; i++) {
            int dx = (int)i - (int)r;
            int dy = (int)j - (int)r;
            if (dx * dx + dy * dy <= r * r) {
                bmp[j][i][0] = color & 0xFF;
                bmp[j][i][1] = (color >> 8) & 0xFF;
                bmp[j][i][2] = (color >> 16) & 0xFF;
            } else {
                bmp[j][i][0] = (maskColor) & 0xFF;
                bmp[j][i][1] = (maskColor >> 8) & 0xFF;
                bmp[j][i][2] = (maskColor >> 16) & 0xFF;
            }
        }
    }

    fbSetRegion(x - r, y - r, d, d, (uint8_t*)bmp, maskColor);
}
void drawChar(uint32_t x, uint32_t y, char ascii, uint32_t color){
	drawCharHighlight(x, y, ascii, color, NO_COLOR_MASK);
}

void drawCharHighlight(uint32_t x, uint32_t y, char ascii, uint32_t color, uint32_t backColor) {
    if ((uint8_t)ascii > 127) return;

    uint8_t bmp[FONT_HEIGHT][FONT_WIDTH][BPP];
    uint32_t maskColor = generateMaskColor(color, backColor);
    int hasBackground = (backColor != NO_COLOR_MASK);  
    
    if(!hasBackground){
        backColor = maskColor;  
    }
    
    for (uint32_t row = 0; row < FONT_HEIGHT; row++) {  
        uint8_t bits = font8x8_basic[(uint8_t)ascii][row];
        for (uint32_t col = 0; col < FONT_WIDTH; col++) { 
            if (bits & (1 << col)) {
                bmp[row][col][0] = color & 0xFF;
                bmp[row][col][1] = (color >> 8) & 0xFF;
                bmp[row][col][2] = (color >> 16) & 0xFF;
            } else {
                bmp[row][col][0] = backColor & 0xFF;
                bmp[row][col][1] = (backColor >> 8) & 0xFF;
                bmp[row][col][2] = (backColor >> 16) & 0xFF;
            }
        }
    }
    
    fbSetRegion(x, y, FONT_WIDTH, FONT_HEIGHT, (uint8_t*)bmp, 
                hasBackground ? NO_COLOR_MASK : maskColor);
}




void drawTextHighlight(uint32_t x, uint32_t y, const char* str, uint32_t color, uint32_t backColor) {
    while (*str) {
        drawCharHighlight(x, y, *str, color, backColor);
        x += FONT_WIDTH;
        str++;
    }
}
void drawText(uint32_t x, uint32_t y, const char* str, uint32_t color) {
	drawTextHighlight(x,y, str, color, NO_COLOR_MASK);
}
void drawInt(uint32_t x, uint32_t y, int value, uint32_t color){
	drawIntHighlight(x,y,value,color, NO_COLOR_MASK);
}
void drawIntHighlight(uint32_t x, uint32_t y, int value, uint32_t color, uint32_t backColor) {
    char buf[TEMP_ALLOC_LEN];
    int i = TEMP_ALLOC_LEN - 1;
    buf[i--] = '\0';
    int negative = 0;

    if (value == 0) {
        buf[i--] = '0';
    } else {
        if (value < 0) {
            negative = 1;
            value = -value;
        }
        while (value && i >= 0) {
            buf[i--] = '0' + (value % 10);
            value /= 10;
        }
        if (negative && i >= 0)
            buf[i--] = '-';
    }

    drawTextHighlight(x, y, &buf[i + 1], color, backColor);
}




// OBSOLETO, NO USAR

// Escribe el char str en la posición (x,y)
/*
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

*/

// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
// Siendo x,y la esquina inferior del rectángulo
//void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
//void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t r);

// Cambia todos los píxeles a hexColor
//void fillScreen(uint32_t hexColor);


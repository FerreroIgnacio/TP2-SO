#include "standard.h"
#include <stdarg.h>


/* MANEJO DE STRINGS */
int strlen(const char *str){
    int len = 0;
    while (*str++) len++;
    return len;
}
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}
int strncmp(const char *s1, const char *s2, uint64_t n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    if (n == (uint64_t)-1) return 0;
    return (unsigned char)*s1 - (unsigned char)*s2;
}
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}
char *strncpy(char *dest, const char *src, uint64_t n) {
    char *ret = dest;
    while (n && (*dest++ = *src++)) n--;
    while (n--) *dest++ = '\0';
    return ret;
}
char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}
char *strncat(char *dest, const char *src, uint64_t n) {
    char *ret = dest;
    while (*dest) dest++;
    while (n-- && (*src)) *dest++ = *src++;
    *dest = '\0';
    return ret;
}


int64_t strtoint(const char *str){
    int64_t sign = 1;
    if (*str == '-') {
        sign = -1; str++; 
    }
    else if (*str == '+') {
        str++; 
    }
    return sign * strtouint(str);
}
uint64_t strtouint(const char *str){
    int64_t toReturn = 0;
    while (*str >= '0' && *str <= '9') {
        toReturn = toReturn * 10 + (*str - '0');
        str++;
    }
    return toReturn;
}
uint64_t strtohex(const char *str){
    unsigned int toReturn = 0;
    while ((*str >= '0' && *str <= '9') ||
           (*str >= 'a' && *str <= 'f') ||
           (*str >= 'A' && *str <= 'F')) {
        toReturn *= 16;
        if (*str >= '0' && *str <= '9') toReturn += *str - '0';
        else if (*str >= 'a' && *str <= 'f') toReturn += *str - 'a' + 10;
        else toReturn += *str - 'A' + 10;
        str++;
    }
    return toReturn;
}
uint64_t strtooct(const char *str){
    unsigned int toReturn = 0;
    while (*str >= '0' && *str <= '7') {
        toReturn = toReturn * 8 + (*str - '0');
        str++;
    }
    return toReturn;
}
// genérico para uint en dec, hex y oct
uint64_t strtoint_complete(const char *str){
    if (str[0] == '0') {
        if (str[1] == 'x' || str[1] == 'X') return strtohex(str + 2);
        else return strtooct(str + 1);
    }
    return strtoint(str);
}




/* FUNCIONALIDADES DEL TECLADO */
// 1 si las (count) teclas de (makecode) estás presionadas, 0 si no. 
int areKeysPressed(int *makecodes, int count) {
    for (int i = 0; i < count; i++) {
        if (!isKeyPressed(makecodes[i])) {
            return 0;
        }
    }
    return 1;
}

// borra todo el file descriptor
void flush(int fd){
    unsigned char buff[STD_BUFF_SIZE];
    read(fd,buff,STD_BUFF_SIZE);
}

/* UTILIDADES STDIN */ 
unsigned char getchar(){
    unsigned char c = 0;
    int count;

    // versión bloqueante
    /*
    do{
        count = read(STDIN, &c, 1);
    }while (!count);
    */
    
    // versión no bloqueante
    count = read(STDIN, &c, 1);
    if (count == 0) return 0;       // si no hay nada para leer, retorna 0
    return c;
    
}

int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int assigned = 0;

    while (*format) {
        if (*format == '%') {
            format++;
            char buffer[STD_BUFF_SIZE];
            int bufIndex = 0;
            unsigned char c;

            // Saltar marcadores de fin
            do {
                c = getchar();
            } while (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\b');

            // Leer token hasta encontrar separador
            // el código es bloqueante, hasta no encontrar un caracter de fin no sigue ejecutando.
            while (c==0||(c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\v' && c != '\b' && bufIndex < STD_BUFF_SIZE - 1)) {
                if(c!=0){
                    buffer[bufIndex++] = c;
                }
                c = getchar();
            }
            buffer[bufIndex] = '\0';

            switch (*format) {
                case 'd': {
                    int *dest = va_arg(args, int *);
                    *dest = strtoint(buffer);
                    assigned++;
                    break;
                }
                case 'u': {
                    unsigned int *dest = va_arg(args, unsigned int *);
                    *dest = strtouint(buffer);
                    assigned++;
                    break;
                }
                case 'X':
                case 'x': {
                    unsigned int *dest = va_arg(args, unsigned int *);
                    *dest = strtohex(buffer);
                    assigned++;
                    break;
                }
                case 'o': {
                    unsigned int *dest = va_arg(args, unsigned int *);
                    *dest = strtooct(buffer);
                    assigned++;
                    break;
                }
                case 'i': {
                    int *dest = va_arg(args, int *);
                    *dest = strtoint_complete(buffer);
                    assigned++;
                    break;
                }
                case 'c': {
                    char *dest = va_arg(args, char *);
                    *dest = buffer[0];
                    assigned++;
                    break;
                }
                case 's': {
                    char *dest = va_arg(args, char *);
                    int i = 0;
                    while (buffer[i]) {
                        dest[i] = buffer[i];
                        i++;
                    }
                    dest[i] = 0;
                    assigned++;
                    break;
                }
                default:
                    break;
            }
        } else {
            format++;
        }

        format++;
    }

    va_end(args);
    return assigned;
}



/* UTILIDADES STDOUT */
void putchar (char c){                          // %c
    char str [1] = {c};
    write(STDOUT,str,1);
}
void puts (const char* str){                    // %s
    write(STDOUT,str,strlen(str));
}
void putuint(uint64_t value){                   // %u
    if (value == 0) {
        putchar('0');
        return;
    }

    char buffer[21];                        // 64bits = 20 digitos (max) + \0
    int i = 0;
     
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);   // 1234 => "4321"
        value /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {      // imprimir en orden correcto
        putchar(buffer[j]);                 
    }
}
void putint(int64_t value){                     // %d %ld %lld
    if (value < 0) {
        putchar('-');
        value = -value;
    }
    putuint(value);
}
void putoct(uint64_t value){                    // %o
    if (value == 0) {
        putchar('0');
        return;
    }

    char buffer[22];                        // 64bits = 21 digitos en octal (max) + \0
    int i = 0;
     
    while (value > 0) {
        buffer[i++] = '0' + (value % 8);    // 1234 => "4321"
        value /= 8;
    }

    for (int j = i - 1; j >= 0; j--) {      // imprimir en orden correcto
        putchar(buffer[j]);                 
    }    
}
void puthex(uint64_t value){                    // %x %p
    if (value == 0) {
        putchar('0');
        return;
    }
    
    char hex_digits [16] = "0123456789abcdef";

    char buffer[16];                        // 64bits = 16 digitos en hexa (max) + \0
    int i = 0;
    
    while (value > 0) {
        buffer[i++] = hex_digits[value % 16];
        value /= 16;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        putchar(buffer[j]);
    }
}
void puthexupper(uint64_t value){               // %X %P
    if (value == 0) {
        putchar('0');
        return;
    }
    
    char hex_digits [16] = "0123456789ABCDEF";

    char buffer[16];                        // 64bits = 16 digitos en hexa (max) + \0
    int i = 0;

    while (value > 0) {
        buffer[i++] = hex_digits[value % 16];
        value /= 16;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        putchar(buffer[j]);
    }
}
// Agregar soporte para double
/*
void putdouble(double value, int precision) {   // %f
    if (value < 0) {
        putchar('-');
        value = -value;
    }

    uint64_t integer = (uint64_t)value;
    putuint(integer);                                   // imprimir parte entera
    putchar('.');
    double fractional = value - integer;

    for (int i = 0; i < precision; i++) {
        fractional *= 10;
    }

    uint64_t decimals = (uint64_t)(fractional + 0.5);   // redondeo

    uint64_t base = 1;                                  // imprimir ceros luego de la coma
    for (int i = 1; i < precision; i++) {
        base *= 10;
        if (decimals < base)
            putchar('0');
    }

    putuint(decimals);                                  // imprimir parte decimal
}
*/
uint64_t printf(const char * format, ...){
    va_list args;
    va_start(args, format);
    char c;
    uint64_t len=0;
    while(* format){
        c = *format++;
        len++;
        switch (c){
            case '%':
                c = *format++;
                switch (c){
                case '%':
                    putchar('%');
                    break;
                case 'c':
                    char ch = (char)va_arg(args, int);
                    putchar(ch);
                    break;
                case 's':
                    puts(va_arg(args,char*));
                    break;
                case 'l':
                    c = *format++;
                    if (c=='d'){
                        putint(va_arg(args,int64_t));
                        break;
                    }
                    if (c=='l'){
                        c = *format++;
                        if (c=='d'){
                            putint(va_arg(args,int64_t));
                            break;
                        }
                        format--;
                    }
                    format--;
                    break;
                case 'd':
                    putint(va_arg(args,int64_t));
                    break;
                case 'u':
                    putuint(va_arg(args,uint64_t));
                    break;
                case '#':
                    c = *format++;
                    if (c == 'x' || c == 'p'){
                        puts("0x");
                        puthex(va_arg(args,uint64_t));
                        break;
                    }
                    if (c == 'X' || c == 'P'){
                        puts("0x");
                        puthexupper(va_arg(args,uint64_t));
                        break;
                    }
                    if (c == 'o'){
                        putchar('0');
                        putoct(va_arg(args,uint64_t));
                        break;
                    }
                    format--;
                    break;
                case 'p':
                case 'x':
                    puthex(va_arg(args,uint64_t));
                    break;   
                case 'P':
                case 'X':
                    puthexupper(va_arg(args,uint64_t));
                    break;   
                case 'o':
                    putoct(va_arg(args,uint64_t));
                    break;

                // Agregar soporte para float / double    
                /*
                case '.':
                    c = *format++;
                    if(c < '0' || c > '9'){
                        format--;
                        break;
                    }
                    int precision = c - '0';
                    c = *format++;
                    if (c != 'f'){
                        format-=2;
                        break;
                    }
                    putdouble(va_arg(args,double),precision);
                    break;
                case 'f':
                    putdouble(va_arg(args,double),6);
                    break;
                default:
                    break;
                */    
                }
                break;
            default:
                putchar(c);
                break;
        }
    }
    return len;
}




/* MANEJO DEL MODO VIDEO */
// Obtener tamaño de la pantalla.
extern void fbSet(uint8_t*);

uint64_t fbGetSize (frame_t* fr){
    return fr->pitch * fr->height;
}

// Inicializa el frame buffer 
frame_t * frameInit(frame_t* fr, uint8_t* fb){
    uint16_t width, height, bpp, pitch;
    getVideoData(&width, &height, &bpp, &pitch);
    fr->frameBuffer = fb;
    fr->width = width;
    fr->height = height;
    fr->bpp = bpp;
    fr->pitch = pitch;
    return fr;
}
// Dibujar un pixel en la posición (x,y) del frame buffer
void framePutPixel(frame_t* fr, uint32_t hexColor, uint64_t x, uint64_t y) {
    uint64_t offset = (x * (fr->bpp/8)) + (y * fr->pitch);
    fr->frameBuffer[offset]     =  (hexColor) & 0xFF;
    fr->frameBuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    fr->frameBuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}
// Dibujar un caracter en la posición (x,y) del frame buffer
void frameDrawChar(frame_t* fr, unsigned char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y) {
    font_info_t currentFont = fontmanager_get_current_font();
    int width = currentFont.width;
    int height = currentFont.height;
    int bytes_per_row = (width + 7) / 8;

    uint8_t *bmp = (uint8_t *)currentFont.data + (ascii * currentFont.bytes_per_char);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int byte_offset = row * bytes_per_row + (col / 8);
            int bit_index;

            // Handle mirrored vs non-mirrored fonts
            if (!currentFont.mirrored) {
                bit_index = col % 8;
            } else {
                bit_index = 7 - (col % 8);  // Reverse bit order for non-mirrored fonts
            }

            int pixel_on = bmp[byte_offset] & (1 << bit_index);
            uint32_t color = pixel_on ? hexColor : backColor;
            framePutPixel(fr, color, x + col, y + row);
        }
    }
}
// Dibujar string en la posición (x,y) del frame buffer
void frameDrawText(frame_t* fr, char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y){
	int i = 0;
	while(str[i] != 0){
		frameDrawChar(fr, (unsigned char)str[i], hexColor, backColor, x, y);
		x+= fontmanager_get_current_font().width; // Avanzar a la siguiente posición
		i++;
	}
}
// Dibujar número en la posición (x,y) del frame buffer
void frameDrawInt(frame_t* fr, int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y){
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
    frameDrawText(fr, buffer, hexColor, backColor, x, y);
}
// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
void frameDrawRectangle(frame_t* fr, uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h){
    for(uint64_t i = 0; i < h; i++){
        for(uint64_t j = 0; j < w; j++){
            uint64_t pixelX = x + j;
            uint64_t pixelY = y + i;

            if(pixelX < fr->width && pixelY < fr->height) {
                framePutPixel(fr,hexColor,pixelX, pixelY);
            }
        }
    }
}

void frameCopyRegion(uint64_t startX, uint64_t startY, uint64_t width, uint64_t height, frame_t* targetFrame, frame_t* sourceFrame) {
    if (!targetFrame || !sourceFrame || width == 0 || height == 0) {
        return;
    }

    // Calculate actual region bounds, clipping to both source and target frames
    uint64_t endX = startX + width;
    uint64_t endY = startY + height;

    // Clip to source frame bounds
    if (endX > sourceFrame->width) {
        endX = sourceFrame->width;
    }
    if (endY > sourceFrame->height) {
        endY = sourceFrame->height;
    }

    // Clip to target frame bounds
    if (endX > targetFrame->width) {
        endX = targetFrame->width;
    }
    if (endY > targetFrame->height) {
        endY = targetFrame->height;
    }

    // If start coordinates are out of bounds, nothing to copy
    if (startX >= endX || startY >= endY) {
        return;
    }

    // Copy pixels byte by byte using bpp
    uint16_t bytesPerPixel = sourceFrame->bpp / 8;

    for (uint64_t y = startY; y < endY; y++) {
        for (uint64_t x = startX; x < endX; x++) {
            uint64_t sourceOffset = (y * sourceFrame->width + x) * bytesPerPixel;
            uint64_t targetOffset = (y * targetFrame->width + x) * bytesPerPixel;

            // Copy pixel byte by byte
            for (uint16_t b = 0; b < bytesPerPixel; b++) {
                targetFrame->frameBuffer[targetOffset + b] = sourceFrame->frameBuffer[sourceOffset + b];
            }
        }
    }
}


void frameCopyCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, frame_t* targetFrame, frame_t* sourceFrame) {
    if (!targetFrame || !sourceFrame || radius == 0) {
        return;
    }

    // Calculate bounding box (handle potential underflow with signed arithmetic)
    int64_t minX_signed = (int64_t)centerX - (int64_t)radius;
    int64_t minY_signed = (int64_t)centerY - (int64_t)radius;

    uint64_t minX = (minX_signed < 0) ? 0 : (uint64_t)minX_signed;
    uint64_t minY = (minY_signed < 0) ? 0 : (uint64_t)minY_signed;
    uint64_t maxX = centerX + radius;
    uint64_t maxY = centerY + radius;

    // Clamp to both source and target frame bounds
    if (maxX > sourceFrame->width) maxX = sourceFrame->width;
    if (maxY > sourceFrame->height) maxY = sourceFrame->height;
    if (maxX > targetFrame->width) maxX = targetFrame->width;
    if (maxY > targetFrame->height) maxY = targetFrame->height;

    // If the bounding box doesn't intersect with the frames, nothing to copy
    if (minX >= maxX || minY >= maxY) {
        return;
    }

    // Copy pixels byte by byte using bpp
    uint16_t bytesPerPixel = sourceFrame->bpp / 8;

    // Copy pixels within circle to same coordinates in target frame
    for (uint64_t y = minY; y < maxY; y++) {
        for (uint64_t x = minX; x < maxX; x++) {
            int64_t dx = (int64_t)x - (int64_t)centerX;
            int64_t dy = (int64_t)y - (int64_t)centerY;

            if (dx * dx + dy * dy <= (int64_t)radius * (int64_t)radius) {
                uint64_t sourceOffset = (y * sourceFrame->width + x) * bytesPerPixel;
                uint64_t targetOffset = (y * targetFrame->width + x) * bytesPerPixel;

                // Copy pixel byte by byte
                for (uint16_t b = 0; b < bytesPerPixel; b++) {
                    targetFrame->frameBuffer[targetOffset + b] = sourceFrame->frameBuffer[sourceOffset + b];
                }
            }
        }
    }
}

static uint64_t rand_seed = 0;
void autoinit() {
    if (rand_seed == 0) {
        rand_seed = getBootTime();
    }
}

uint32_t rand() {
    autoinit();
    // LCG formula
    rand_seed = rand_seed * 1664525ULL + 1013904223ULL;
    return (uint32_t)(rand_seed >> 32);
}



// Dibuja un círculo de r píxeles de radio en la posición (x,y)
void frameDrawCircle(frame_t* fr, uint32_t hexColor, uint64_t x, uint64_t y, int64_t r){

    for(signed int dx = -r; dx <= r; dx++){
        for(int dy = -r; dy <= r; dy++){
            if(dx * dx + dy * dy <= r * r){
                uint64_t pixelX = x + dx;
                uint64_t pixelY = y + dy;
                if(pixelX >= 0 && pixelY >= 0 && pixelX < fr->width && pixelY < fr->height) {
                    framePutPixel(fr,hexColor,pixelX, pixelY);
                }
	    }
	}
   }
}
// Llenar el frame bufer con hexColor
void frameFill(frame_t* fr, uint32_t hexColor){
    uint8_t r = (hexColor >> 16) & 0xFF;
    uint8_t g = (hexColor >> 8) & 0xFF;
    uint8_t b = hexColor & 0xFF;
    uint8_t bytesPerPixel = fr->bpp / 8;
    
    for (uint16_t y = 0; y < fr->height; y++) {
        for (uint16_t x = 0; x < fr->width; x++) {
            uint64_t offset = y * fr->pitch + x * bytesPerPixel;
            fr->frameBuffer[offset] = b;
            fr->frameBuffer[offset + 1] = g;
            fr->frameBuffer[offset + 2] = r;
        }
    }
}

void setFrame(frame_t* fr){
    fbSet(fr->frameBuffer);
}

/* CÁLCULO DE FPS */ 
uint64_t framesCount, timerCount;
void fpsInit(){
    timerCount = getBootTime();
    framesCount = 0;
}

void incFramesCount(){
    framesCount++;
}
// Promedio de Frames Por Segundo desde la úĺtima llamada
uint64_t getFps(){
    uint64_t time = getBootTime()-timerCount;
    if (time < 100) return 0;
    uint64_t fps  = (1000 * framesCount)/time;
    fpsInit();
    return fps;
};

/* SONIDO */
void playFreq(uint16_t freq, uint64_t ms){
    startSound(freq);
    uint64_t start = getBootTime();
    while (getBootTime() - start < ms);
    stopSound();
}

void setFrameMasked(frame_t * fr, uint64_t maskColor) {
    fbSetRegion(0,0, fr->width, fr->height, fr->frameBuffer, maskColor);
}
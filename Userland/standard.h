#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
#include <stdarg.h>
#include "fontManager.h"

#define FONT_HEIGHT 8
#define FONT_WIDTH 8

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STD_BUFF_SIZE 4096

/* MANEJO DE STRINGS */
int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint64_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, uint64_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, uint64_t n);

int64_t strtoint(const char *str);
uint64_t strtouint(const char *str);
uint64_t strtohex(const char *str);
uint64_t strtooct(const char *str);
// genérico para uint en dec, hex y oct
uint64_t strtoint_complete(const char *str);

/* UTILIDADES FILE DESCRIPTORS */
// Escribir (count) caracteres de (buf) en el file descriptor (fd).
int write(int fd, const char *buff, unsigned long count);
// Leer (count) caracteres del file descriptor (fd) y guardar en (buf).
int read(int fd, unsigned char *buff, unsigned long count);



/* FUNCIONALIDADES DEL TECLADO */
// 1 si la tecla (makecode) está presionada, 0 si no. 
// para scancodes especiales agregar E0, por ejemplo, 0xE048 para flecha arriba
int isKeyPressed(uint16_t makecode);
// 1 si las (count) teclas de (makecode) estás presionadas, 0 si no. 
int areKeysPressed(int * makecodes, int count);



/* TIME */
// Tiempo de booteo en ms
uint64_t getBootTime();
// Guarda el tiempo según el reloj interno del dispositivo (por lo general, UTC 0)
void getLocalTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);
// Guarda la fecha según el reloj interno del dispositivo (por lo general, UTC 0)
void getLocalDate(uint8_t* year, uint8_t* month, uint8_t* day);



/* CAPTURA DE REGISTROS */
// Se define struct registers para mejorar la sintaxis.
typedef struct registers{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t rip;
} registers_t;
// Guarda en regs la captura de los registros realizada con F1
void getRegisters(registers_t* regs);



/* UTILIDADES STDIN */ 
unsigned char getchar();
int scanf(const char *format, ...);


/* UTILIDADES STDOUT */
void putchar (char c);              // %c
void puts (const char* str);        // %s
void putuint(uint64_t c);           // %u
void putint(int64_t c);             // %d %ld %lld
void putoct(uint64_t c);            // %o
void puthex(uint64_t c);            // %x %p
void puthexupper(uint64_t value);   // %X %P
/* Falta agregar soporte para double */
//void putdouble(double value, int precision); //%f
uint64_t printf(const char * format, ... );



/* MANEJO DEL MODO VIDEO */
typedef struct videoData{
    uint8_t * fb;
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint16_t pitch;
} videoData_t;
// Cambia el color del pixel (x,y) a hexColor
extern void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
// Guarda en los punteros los datos de la pantalla.
// Si no se quiere guardar un dato, colocar NULL como argumento;
extern void getVideoData(uint16_t* width, uint16_t* height, uint16_t* bpp, uint16_t* pitch);
// retorna el tamaño del Framebuffer
uint64_t fbGetSize ();
// Cambia el pixel (x,y) del frame buffer por hexColor
void fbPutPixel(uint8_t * fb, uint32_t hexColor, uint64_t x, uint64_t y, uint64_t bpp, uint64_t pitch);
// Dibujar caracter en la posición (y,x) del frame buffer
void fbDrawChar(uint8_t * fb, unsigned char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y);
// Dibujar string en la posición (y,x) del frame buffer
void fbDrawText(uint8_t * fb, char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y);
// Dibujar número en la posición (y,x) del frame buffer
void fbDrawInt(uint8_t * fb, int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y);
// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
void fbDrawRectangle(uint8_t * fb, uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
// Dibuja un círculo de r píxeles de radio en la posición (x,y)
void fbDrawCircle(uint8_t * fb, uint32_t hexColor, uint64_t x, uint64_t y, int64_t r);
// Llenar el frame bufer con hexColor
void fbFill (uint8_t * fb, uint32_t hexColor);
// Muestra fb en pantalla. fb es un vector de al menos fbGetSize() posiciones
void fbSet(uint8_t * fb);


/* CÁLCULO DE FPS */ 
// Inicia el contador de frames
void fpsInit();
// Promedio de Frames Por Segundo desde la úĺtima llamada a getFps()
uint64_t getFps();





/* MANEJO DEL MODO VIDEO V2 */
/******NO USAR ESTA PARTE******/
// 
void fbSetRegion(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, uint8_t* bmp, uint32_t maskColor);
// Dibuja un pixel en (x, y) con color RGB
void setPixel(uint32_t x, uint32_t y, uint32_t color);
// Dibuja un rectángulo de tamaño w x h en (x, y)
void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
// Dibuja un círculo centrado en (x, y) con radio r
void drawCircle(uint32_t x, uint32_t y, uint32_t r, uint32_t color);
// Dibuja el carácter ASCII c (fuente 8x8) en (x, y)
void drawChar(uint32_t x, uint32_t y, char c, uint32_t color);
// Dibuja texto plano en (x, y) (usa fuente 8x8)
void drawText(uint32_t x, uint32_t y, const char* str, uint32_t color);
// Dibuja un entero como texto decimal en (x, y)
void drawInt(uint32_t x, uint32_t y, int value, uint32_t color);
/* Iguales a las de arriba, pero utilizando backColor */
void drawCharHighlight(uint32_t x, uint32_t y, char c, uint32_t color, uint32_t backColor);
void drawTextHighlight(uint32_t x, uint32_t y, const char* str, uint32_t color, uint32_t backColor);
void drawIntHighlight(uint32_t x, uint32_t y, int value, uint32_t color, uint32_t backColor);


#endif

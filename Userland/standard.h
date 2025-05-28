#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
//#include "font8x8/font8x8_basic.h"
#define NULL 0
#define FONT_HEIGHT 8
#define FONT_WIDTH 8

extern char font8x8_basic[128][8];

extern int syscall_write(int fd, const char *buf, unsigned long count);
extern int syscall_read(int fd, char *buf, unsigned long count);
extern int syscall_isKeyDown(int scancode);

// devuelve el tiempo de booteo en ms
uint64_t getBootTime();

void getLocalTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

void getLocalDate(uint8_t* year, uint8_t* month, uint8_t* day);

char getAsciiFromMakeCode(uint8_t makeCode, int shifted);

// Cambia el color del pixel (x,y) a hexColor
extern void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

// Guarda en los punteros los datos de la pantalla.
// Si no se quiere guardar un dato, colocar NULL como argumento;
extern void getVideoData(uint16_t* width, uint16_t* height, uint16_t* bpp, uint16_t* pitch);

// retorna el tamaño del Framebuffer
uint64_t fbGetSize ();

// fb es un vector de getFramebufferSize() posiciones
void fbSet(uint8_t * fb);

void fbDrawChar(uint8_t * fb, char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

void fbDrawText(uint8_t * fb, char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

void fbDrawInt(uint8_t * fb, int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

void fbFill (uint8_t * fb, uint32_t hexColor);

void incFramesCount();
void fpsInit();

//retorna la cantidad de frames 
uint64_t getFps();

void itos(uint64_t value, char* str) ;

// Función para formato de fecha/hora con padding de ceros
void itos_padded(uint64_t value, char* str, int width);



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

#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
//#include "font8x8/font8x8_basic.h"
extern char font8x8_basic[128][8];

extern int syscall_write(int fd, const char *buf, unsigned long count);
extern int syscall_read(int fd, char *buf, unsigned long count);
extern int syscall_isKeyDown(int scancode);

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

void itos(uint64_t value, char* str) ;

// Función para formato de fecha/hora con padding de ceros
void itos_padded(uint64_t value, char* str, int width);



// OBSOLETO, NO USAR
/*

void drawChar(char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

// Escribe el string str en la posición (x,y)
void drawText(char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

void drawInt(int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

*/

// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
// Siendo x,y la esquina inferior del rectángulo
//void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
//void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t r);

// Cambia todos los píxeles a hexColor
//void fillScreen(uint32_t hexColor);



#endif

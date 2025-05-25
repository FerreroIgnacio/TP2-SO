#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
//#include "font8x8/font8x8_basic.h"
extern char font8x8_basic[128][8];

// Cambia el color del pixel (x,y) a hexColor
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

//uint16_t getWidth();
//uint16_t getHeight();

void drawChar(char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

// Escribe el string str en la posición (x,y)
void drawText(char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

void drawInt(int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

char getAsciiFromMakeCode(uint8_t makeCode, int shifted);

// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
// Siendo x,y la esquina inferior del rectángulo
//void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
//void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t r);

// Cambia todos los píxeles a hexColor
//void fillScreen(uint32_t hexColor);

extern int syscall_write(int fd, const char *buf, unsigned long count);
extern int syscall_read(int fd, char *buf, unsigned long count);
extern int syscall_isKeyDown(int scancode);
extern int syscall_time();

#endif

#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

typedef struct frameBuffer *frameBuffer_adt;

frameBuffer_adt getFB();

void setFB(uint8_t index);

void freeFB(uint8_t index);

// Cambia el color del pixel (x,y) a hexColor
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

// Escribe el string str en la posición (x,y)
void putText(char *str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
// Siendo x,y la esquina inferior del rectángulo
void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, int64_t r);

// Cambia todos los píxeles a hexColor
void fillScreen(uint32_t hexColor);

void drawInt(int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);

void putChar(char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size);
#endif

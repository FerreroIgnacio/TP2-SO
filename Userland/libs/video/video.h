#ifndef VIDEO_H
#define VIDEO_H
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "../fontManager/fontManager.h"

typedef struct frameBuffer_struct *frameBuffer;

// crea un nuevo frameBuffer
frameBuffer getFB();

// muestra el frameBuffer en pantalla
void setFB(frameBuffer fb);

// libera el frameBuffer
void freeFB(frameBuffer fb);

uint8_t frameGetWidth(frameBuffer fb);

uint8_t frameGetHeight(frameBuffer fb);

uint8_t frameGetBpp(frameBuffer fb);

uint32_t frameGetPitch(frameBuffer fb);

// Cambia el pixel (x,y) del frame por hexColor
void framePutPixel(frameBuffer fb, uint32_t hexColor, uint64_t x, uint64_t y);

// Dibujar caracter en la posición (y,x) del frame
void frameDrawChar(frameBuffer fb, unsigned char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y);

// Dibujar string en la posición (y,x) del frame
void frameDrawText(frameBuffer fb, char *str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y);

// Dibujar número en la posición (y,x) del frame
void frameDrawInt(frameBuffer fb, int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y);

// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y) del frame
void frameDrawRectangle(frameBuffer fb, uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y) del frame
void frameDrawCircle(frameBuffer fb, uint32_t hexColor, uint64_t x, uint64_t y, int64_t r);

// Llenar el frame con hexColor
void frameFill(frameBuffer fb, uint32_t hexColor);

void frameCopyRegion(uint64_t startX, uint64_t startY, uint64_t width, uint64_t height, frameBuffer targetFrame, frameBuffer sourceFrame);

void frameCopyCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, frameBuffer targetFrame, frameBuffer sourceFrame);

#endif

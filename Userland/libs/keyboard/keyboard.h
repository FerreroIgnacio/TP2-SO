#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>
/* FUNCIONALIDADES DEL TECLADO */
// 1 si la tecla (makecode) está presionada, 0 si no.
// para scancodes especiales agregar E0, por ejemplo, 0xE048 para flecha arriba
int isKeyPressed(uint16_t makecode);
// 1 si las (count) teclas de (makecode) estás presionadas, 0 si no.
int areKeysPressed(int *makecodes, int count);


#endif
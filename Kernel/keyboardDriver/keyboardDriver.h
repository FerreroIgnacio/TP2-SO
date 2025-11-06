
#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#include <stdint.h>

int isKeyPressed(uint16_t makeCode);
int areKeysPressed(uint16_t *makeCodeVec);
void setKeyPressed(uint8_t makeCode);
void setKeyReleased(uint8_t makeCode);

#endif

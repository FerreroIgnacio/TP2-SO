
#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#include <stdint.h>
#include <keyboardDriver.h>
#include <idtInit.h>        
#include <stdin.h>
#include <videoDriver.h>

int isKeyPressed(uint16_t makeCode);
int areKeysPressed(uint16_t * makeCodeVec);
void setKeyPressed(uint8_t makeCode);
void setKeyReleased(uint8_t makeCode);



#endif


#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#include <stdint.h>
#include <keyboardDriver.h>
#include <idtLoader.h>        
#include <stdint.h>
#include <videoDriver.h>

int isKeyPressed(uint8_t scanCode);
int areKeyPressed(uint8_t * scanCodeVec);
void setKeyPressed(uint8_t scanCode);
void setKeyReleased(uint8_t scanCode);



#endif

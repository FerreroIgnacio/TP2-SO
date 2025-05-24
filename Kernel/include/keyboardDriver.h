
#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#include <stdint.h>
#include <keyboardDriver.h>
#include <idtLoader.h>        // ADD THIS LINE
#include <stdint.h>
#include <videoDriver.h>
//char getPressedKey();

typedef void (*keyboard_handler_t)(uint8_t scancode);

void setKeyboardHandler(keyboard_handler_t new_handler);


int isKeyPressed(uint8_t scanCode);
int areKeyPressed(uint8_t * scanCodeVec);
void setKeyPressed(uint8_t scanCode);
void setKeyReleased(uint8_t scanCode);



#endif

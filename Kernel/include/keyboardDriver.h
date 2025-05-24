#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#include <stdint.h>
#include <keyboardDriver.h>
#include <idtLoader.h>        // ADD THIS LINE
#include <stdint.h>
#include <videoDriver.h>
//char getPressedKey();

void keyboard_init(void);


typedef void (*keyboard_handler_t)(uint8_t scancode);

void setKeyboardHandler(keyboard_handler_t new_handler);

extern char keysDown[256];

#endif

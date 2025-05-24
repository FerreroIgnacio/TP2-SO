#include <keyboardDriver.h>
#include <idtLoader.h>   
#include <stdint.h>
#include <videoDriver.h>

#define KB_BUFFER_SIZE 128


static char buffer[KB_BUFFER_SIZE];
char keysDown[256] = {0};
/*
 * funcion declarada en keyboardIH.asm
 */
keyboard_handler_t current_handler = 0;
extern void keyboard_interrupt_handler(void);
void keyboard_init(){
    IDTadd(0x21, keyboard_interrupt_handler, 0x8E);  // Standard
}


    void setKeyboardHandler(keyboard_handler_t new_handler) {
    current_handler = new_handler;
}

void keyPressedAction(uint8_t scancode) {
	uint8_t keycode = scancode & 0x7F;
    
    // 0x80 si se suelta una tecla
    if (scancode & 0x80) {
        keysDown[keycode] = 0;  // Se suelta tecla
    } else {
        keysDown[keycode] = 1;  // Se apreta tecla
    }
    
    // Paso la key a lo que sea que tenga el control (si hay)
    if (current_handler != 0) {
        current_handler(scancode); 
    }
}
static int shiftPressed = 0;

char KB_getNextCode(){
	static uint32_t nextKey = 0;
	uint32_t toRet = buffer[nextKey++];
	if (nextKey >= KB_BUFFER_SIZE){
		nextKey = 0;
	}
	return toRet;
}

char KB_getNextAscii(){
	char toRet = 'a'; //TODO
	return toRet;
}

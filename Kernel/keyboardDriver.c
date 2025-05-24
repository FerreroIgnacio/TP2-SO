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
//    putChar(scancode, 0xFFFFFF, 0x000000, 0, 0, 5);
   // while(1);
}
int areKeysDown(char * asciiVec){
	/*
	 * Implementar aca :)
	 */
  if (asciiVec == 0) {
        return 0;
    }

    // Iterar a través del vector hasta encontrar un 0 (terminador)
    for (int i = 0; asciiVec[i] != 0; i++) {
        int ascii = asciiVec[i];

        // Verificar si la tecla NO está presionada
        if (!isKeyDown(ascii)) {
            return 0;  // Si cualquier tecla no está presionada, retornar false
        }
    }

    return 1;  // Todas las teclas están presionadas
}
int isKeyDown(int ascii){
	return keysDown[ascii];
}
void setKeyDown(int ascii){
	keysDown[ascii] = 1;
}
void setKeyUp(int ascii){
        keysDown[ascii] = 0;
}

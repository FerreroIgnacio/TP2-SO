#include <keyboardDriver.h>
#include <idtLoader.h>   
#include <stdint.h>
#include <videoDriver.h>


// Vector de teclas presionadas
static char keysDown[256] = {0};

// Handler actual
keyboard_handler_t current_handler = 0;

// Guarda un nuevo keyboard handler
void setKeyboardHandler(keyboard_handler_t new_handler) {
    current_handler = new_handler;
}


// Handler de la interrupción irq01
void keyPressedAction(uint8_t scancode) {

    uint8_t keycode = scancode & 0x7F;

    // 0x80 si se suelta una tecla
    if (scancode & 0x80) {
        keysDown[keycode-0x80] = 0;  // Se suelta tecla
    } else {
        keysDown[keycode] = 1;  // Se apreta tecla
    }

    // Paso la key a lo que sea que tenga el control (si hay)
    if (current_handler != 0) {
	current_handler(scancode); 
    }
   // putChar(scancode, 0xFFFFFF, 0x000000, 0, 0, 5);
   // while(1);
}

int areKeysPressed(uint8_t * scanCodeVec){
  if (scanCodeVec == 0) {
        return 0;
    }

    // Iterar a través del vector hasta encontrar un 0 (terminador)
    for (int i = 0; scanCodeVec[i] != 0; i++) {
        // Verificar si la tecla NO está presionada
        if (!isKeyPressed(scanCodeVec[i])) {
            return 0;  // Si cualquier tecla no está presionada, retornar false
        }
    }

    return 1;  // Todas las teclas están presionadas
}
int isKeyPressed(uint8_t scanCode){
	return keysDown[scanCode];
}
void setKeyPressed(uint8_t scanCode){
	keysDown[scanCode] = 1;
}
void setKeyReleased(uint8_t scanCode){
        keysDown[scanCode] = 0;
}

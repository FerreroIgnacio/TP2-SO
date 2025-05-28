#include <keyboardDriver.h>
#include <idtLoader.h>   
#include <stdint.h>
#include <videoDriver.h>


// Vector de teclas presionadas
static char keysDown[256] = {0};

// Handler de la interrupción irq01
static int shiftPressed = 0;
static int capsLockOn = 0;
void keyPressedAction(uint8_t scancode) {
    uint8_t keycode = scancode & 0x7F;
    
    if (scancode & 0x80) {
        keysDown[keycode] = 0;
    } else {
        keysDown[keycode] = 1;
	
	if (keycode == 0x3A) {  // Caps Lock scancode
            capsLockOn = !capsLockOn;
        }
		
        // Solo mostrar y enviar scancode crudo
	  drawInt(scancode, 0xFFFFFF, 0x000000, 0, getHeight() - 8 * 3, 3);
    //	char * msj = sys_isKeyDown(0x2A) || sys_isKeyDown(0x36) ? "Algun shift down :)" : "Ningun shift down :(";
    //	putText(msj, 0xFFFFFF, 0x000000, 0, getHeight() - 8 * 3 - 8 * 3, 3);
	//        putChar(scancode, 0xFFFFFF, 0x000000, 0, getHeight() - 8 * 3, 3);
        queueKey(scancode);
    }
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

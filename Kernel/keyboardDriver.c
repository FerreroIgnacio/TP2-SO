#include <keyboardDriver.h>
#include <idtInit.h>   
#include <stdint.h>
#include <videoDriver.h>
#include <syscalls.h>

#define KEYCODE_BUFFER_SIZE 128

#define CTRL_MAKECODE 0x1D
#define R_MAKECODE 0x13


// Vector de teclas presionadas
static char keysPressed[KEYCODE_BUFFER_SIZE*2] = {0};
int extended = 0;

// Handler de la interrupción irq01
//static int shiftPressed = 0;
static int capsLockOn = 0;
void keyPressedAction(uint8_t makecode) {
    
    if (makecode == 0xE0){
        extended++;
        return;
    }

    uint8_t keycode = makecode & 0x7F;

    if (extended){
        if (makecode & 0x80) {
            keysPressed[keycode + KEYCODE_BUFFER_SIZE] = 0;

        } else {
            keysPressed[keycode + KEYCODE_BUFFER_SIZE] = 1;
            // KeyCombo save registers
            if (makecode == CTRL_MAKECODE && isKeyPressed(R_MAKECODE)){
                sys_saveRegisters();
            }
            queueKeyStdin(0xE0);
            queueKeyStdin(makecode);
        }
        extended--;
    }else{
        if (makecode & 0x80) {
            keysPressed[keycode] = 0;
        } else {
            keysPressed[keycode] = 1;
            
            // KeyCombo save registers
            
            if ((   makecode == R_MAKECODE || makecode == CTRL_MAKECODE) &&
                    (isKeyPressed(CTRL_MAKECODE) || isKeyPressed (CTRL_MAKECODE + KEYCODE_BUFFER_SIZE)) && 
                    isKeyPressed(R_MAKECODE)  ) {
                sys_saveRegisters();
            }

            if (keycode == 0x3A) {  // Caps Lock makecode
                capsLockOn = !capsLockOn;
            }
            queueKeyStdin(makecode);
        }
    }
}
int areKeysPressed(uint16_t * makeCodeVec){
  if (makeCodeVec == 0) {
        return 0;
    }
    for (int i = 0; makeCodeVec[i] != 0; i++) {
        if (!isKeyPressed(makeCodeVec[i])) {
            return 0; 
        }
    }
    return 1;
}
int isKeyPressed(uint16_t makeCode){
    if (makeCode & 0xE000){ // Si se recibió un makecode especial
        return keysPressed[(makeCode & 0x7F) + KEYCODE_BUFFER_SIZE];
    }
	return keysPressed[makeCode & 0x7F];
}
void setKeyPressed(uint8_t makeCode){
	keysPressed[makeCode] = 1;
}
void setKeyReleased(uint8_t makeCode){
        keysPressed[makeCode] = 0;
}

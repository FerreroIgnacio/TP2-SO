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

// Make code to ASCII mapping for Set 1 scancodes
char makeCodeToAscii[128] = {
    0,      // 0x00 - unused
    0,      // 0x01 - ESC
    '1',    // 0x02
    '2',    // 0x03
    '3',    // 0x04
    '4',    // 0x05
    '5',    // 0x06
    '6',    // 0x07
    '7',    // 0x08
    '8',    // 0x09
    '9',    // 0x0A
    '0',    // 0x0B
    '-',    // 0x0C
    '=',    // 0x0D
    0,      // 0x0E - Backspace
    0,      // 0x0F - Tab
    'q',    // 0x10
    'w',    // 0x11
    'e',    // 0x12
    'r',    // 0x13
    't',    // 0x14
    'y',    // 0x15
    'u',    // 0x16
    'i',    // 0x17
    'o',    // 0x18
    'p',    // 0x19
    '[',    // 0x1A
    ']',    // 0x1B
    0,      // 0x1C - Enter
    0,      // 0x1D - Left Ctrl
    'a',    // 0x1E
    's',    // 0x1F
    'd',    // 0x20
    'f',    // 0x21
    'g',    // 0x22
    'h',    // 0x23
    'j',    // 0x24
    'k',    // 0x25
    'l',    // 0x26
    ';',    // 0x27
    '\'',   // 0x28
    '`',    // 0x29
    0,      // 0x2A - Left Shift
    '\\',   // 0x2B
    'z',    // 0x2C
    'x',    // 0x2D
    'c',    // 0x2E
    'v',    // 0x2F
    'b',    // 0x30
    'n',    // 0x31
    'm',    // 0x32
    ',',    // 0x33
    '.',    // 0x34
    '/',    // 0x35
    0,      // 0x36 - Right Shift
    '*',    // 0x37 - Keypad *
    0,      // 0x38 - Left Alt
    ' ',    // 0x39 - Space
    0,      // 0x3A - Caps Lock
    0,      // 0x3B - F1
    0,      // 0x3C - F2
    0,      // 0x3D - F3
    0,      // 0x3E - F4
    0,      // 0x3F - F5
    0,      // 0x40 - F6
    0,      // 0x41 - F7
    0,      // 0x42 - F8
    0,      // 0x43 - F9
    0,      // 0x44 - F10
    0,      // 0x45 - Num Lock
    0,      // 0x46 - Scroll Lock
    '7',    // 0x47 - Keypad 7
    '8',    // 0x48 - Keypad 8
    '9',    // 0x49 - Keypad 9
    '-',    // 0x4A - Keypad -
    '4',    // 0x4B - Keypad 4
    '5',    // 0x4C - Keypad 5
    '6',    // 0x4D - Keypad 6
    '+',    // 0x4E - Keypad +
    '1',    // 0x4F - Keypad 1
    '2',    // 0x50 - Keypad 2
    '3',    // 0x51 - Keypad 3
    '0',    // 0x52 - Keypad 0
    '.',    // 0x53 - Keypad .
};

// Shifted characters mapping
char makeCodeToAsciiShifted[128] = {
    0,      // 0x00 - unused
    0,      // 0x01 - ESC
    '!',    // 0x02 - Shift+1
    '"',    // 0x03 - Shift+2 (assuming US-International)
    '#',    // 0x04 - Shift+3
    '$',    // 0x05 - Shift+4
    '%',    // 0x06 - Shift+5
    '&',    // 0x07 - Shift+6
    '/',    // 0x08 - Shift+7
    '(',    // 0x09 - Shift+8
    ')',    // 0x0A - Shift+9
    '=',    // 0x0B - Shift+0 (assuming some layouts)
    '_',    // 0x0C - Shift+-
    '?',    // 0x0D - Shift+=
    0,      // 0x0E - Backspace
    0,      // 0x0F - Tab
    'Q',    // 0x10
    'W',    // 0x11
    'E',    // 0x12
    'R',    // 0x13
    'T',    // 0x14
    'Y',    // 0x15
    'U',    // 0x16
    'I',    // 0x17
    'O',    // 0x18
    'P',    // 0x19
    '{',    // 0x1A - Shift+[
    '}',    // 0x1B - Shift+]
    0,      // 0x1C - Enter
    0,      // 0x1D - Left Ctrl
    'A',    // 0x1E
    'S',    // 0x1F
    'D',    // 0x20
    'F',    // 0x21
    'G',    // 0x22
    'H',    // 0x23
    'J',    // 0x24
    'K',    // 0x25
    'L',    // 0x26
    ':',    // 0x27 - Shift+;
    '"',    // 0x28 - Shift+'
    '¨',    // 0x29 - Shift+` (diaeresis in some layouts)
    0,      // 0x2A - Left Shift
    '|',    // 0x2B - Shift+\
    'Z',    // 0x2C
    'X',    // 0x2D
    'C',    // 0x2E
    'V',    // 0x2F
    'B',    // 0x30
    'N',    // 0x31
    'M',    // 0x32
    '<',    // 0x33 - Shift+,
    '>',    // 0x34 - Shift+.
    '¿',    // 0x35 - Shift+/ (inverted question in Spanish layouts)
    0,      // 0x36 - Right Shift
    '*',    // 0x37 - Keypad *
    0,      // 0x38 - Left Alt
    ' ',    // 0x39 - Space
    0,      // 0x3A - Caps Lock
    0,      // 0x3B - F1
    0,      // 0x3C - F2
    0,      // 0x3D - F3
    0,      // 0x3E - F4
    0,      // 0x3F - F5
    0,      // 0x40 - F6
    0,      // 0x41 - F7
    0,      // 0x42 - F8
    0,      // 0x43 - F9
    0,      // 0x44 - F10
    0,      // 0x45 - Num Lock
    0,      // 0x46 - Scroll Lock
    '7',    // 0x47 - Keypad 7
    '8',    // 0x48 - Keypad 8
    '9',    // 0x49 - Keypad 9
    '-',    // 0x4A - Keypad -
    '4',    // 0x4B - Keypad 4
    '5',    // 0x4C - Keypad 5
    '6',    // 0x4D - Keypad 6
    '+',    // 0x4E - Keypad +
    '1',    // 0x4F - Keypad 1
    '2',    // 0x50 - Keypad 2
    '3',    // 0x51 - Keypad 3
    '0',    // 0x52 - Keypad 0
    '.',    // 0x53 - Keypad .
};

char getAsciiFromMakeCode(uint8_t makeCode, int shifted) {
    if (makeCode >= 128) return 0;
    return shifted ? makeCodeToAsciiShifted[makeCode] : makeCodeToAscii[makeCode];
}
// Handler de la interrupción irq01
static int shiftPressed = 0;
// Handler de la interrupción irq01
void keyPressedAction(uint8_t scancode) {
    uint8_t keycode = scancode & 0x7F;
    
    // Detectar shift
    int shiftPressed = keysDown[0x2A] || keysDown[0x36];
    
    // Obtener ASCII (puede ser 0 si no es imprimible)
    char asciiChar = getAsciiFromMakeCode(keycode, shiftPressed);
    
    // Manejar key up / key down
    if (scancode & 0x80) {
        keysDown[keycode] = 0;
    } else {
        keysDown[keycode] = 1;
        
        // Procesar solo key press - SIN FILTROS
        if (asciiChar != 0) {
            queueKey(asciiChar);
            putChar(asciiChar, 0xFFFFFF, 0x000000, 0, getHeight() - 8 * 3, 3);
        } else {
            drawInt(keycode, 0xFFFFFF, 0x000000, 0, getHeight() - 8 * 3, 3);
        }
    }
    
    if (current_handler != 0) {
        //current_handler(scancode);
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

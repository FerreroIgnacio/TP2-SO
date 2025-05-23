#include <keyboardDriver.h>
#include <stdint.h>
#include <videoDriver.h>
#define KB_BUFFER_SIZE 128


static char buffer[KB_BUFFER_SIZE];

/*
 * funcion declarada en keyboardIH.asm
 */
extern void keyboard_interrupt_handler(void);
void keyboard_init(){
	IDTadd(0x21, keyboard_interrupt_handler, 0x8E);
}
 char scancodeToAscii(uint8_t scancode);

 int index = 0;
void keyPressedAction (char scancode){
	if (scancode & 0x80)
        return;  // Ignorar break code (tecla soltada)

    static uint32_t nextPos = 0;
    buffer[nextPos++] = scancode;
    if (nextPos >= KB_BUFFER_SIZE) nextPos = 0;
    buffer[nextPos] = 0;

    char ascii = scancodeToAscii(scancode);
    if (ascii){
        putChar(ascii, 0xFF0000, 0x000000, 30 + (index * 8 * 5), 50, 5);
    	index++;
    }

}

static int shiftPressed = 0;

char scancodeToAscii(uint8_t scancode) {
    static const char sc_ascii[] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',     // 0x00-0x0E
       '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',         // 0x0F-0x1C
        0,  'a','s','d','f','g','h','j','k','l',';','\'','`',              // 0x1D-0x29
        0,  '\\','z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' '   // 0x2A-0x39
    };

    static const char sc_ascii_shift[] = {
        0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
       '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
        0,  'A','S','D','F','G','H','J','K','L',':','"','~',
        0,  '|','Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' '
    };

    // Manejo de Shift Pressed / Released
    if (scancode == 0x2A || scancode == 0x36) {  // Left or Right Shift press
        shiftPressed = 1;
        return 0;
    } else if (scancode == 0xAA || scancode == 0xB6) { // Shift release
        shiftPressed = 1;
        return 0;
    }

    if (scancode > 0x39)
        return 0;

    return shiftPressed ? sc_ascii_shift[scancode] : sc_ascii[scancode];
}

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

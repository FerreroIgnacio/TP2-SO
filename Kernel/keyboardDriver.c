#include <keyboardDriver.h>
#include <stdint.h>

#define KB_BUFFER_SIZE 128


static char buffer[KB_BUFFER_SIZE] = 0;

// Función para el IRQ Handler
// Guarda los keycodes en un buffer circular
// es importante leerlo antes de que el usuario
// sobreescriba el buffer
void keyPressedAction (char code){
	static uint32_t nextPos = 0;
	buffer[nextPos++] = code;
	if (nextPos >= KB_BUFFER_SIZE){
		nextPos = 0;
	}
	buffer[nextPos]=0;
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

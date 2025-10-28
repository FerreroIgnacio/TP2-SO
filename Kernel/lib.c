#include <stdint.h>
#include <lib.h>

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

void itos(uint64_t value, char* str) {
    // Caso especial: si el valor es 0
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // Buffer temporal para construir el string al revés
    char temp[21]; // máximo 20 dígitos para uint64_t + null terminator
    int index = 0;
    
    // Extraer dígitos (quedan en orden inverso)
    while (value > 0) {
        temp[index] = (value % 10) + '0';
        value /= 10;
        index++;
    }
    
    // Copiar al string destino en orden correcto
    int i;
    for (i = 0; i < index; i++) {
        str[i] = temp[index - 1 - i];
    }
    str[i] = '\0'; // null terminator
}

void itos_padded(uint64_t value, char* str, int width) {

    char temp[21];
    itos(value, temp);
    
    int len = 0;
    while (temp[len] != '\0') len++;
    
    int padding = width - len;
    int i;
    for (i = 0; i < padding; i++) {
        str[i] = '0';
    }
    
    for (i = 0; i < len; i++) {
        str[padding + i] = temp[i];
    }
    
    // no agrega null
}

void uint64ToHex(uint64_t value, char *buffer) {
    const char hexDigits[] = "0123456789ABCDEF";
    for (int i = 17; i >= 2; i--) {
        buffer[i] = hexDigits[value & 0xF]; 
        value >>= 4;                         
    }
	buffer[0] = '0';
	buffer[1] = 'x';
    buffer[18] = '\0';
}
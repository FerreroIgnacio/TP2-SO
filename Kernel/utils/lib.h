#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

void itos(uint64_t value, char* str);

void itos_padded(uint64_t value, char* str, int width);

// buffer size >= 19 (respuesta = 0x0123456789ABCDEF\0 )
void uint64ToHex(uint64_t value, char *buffer) ;

#endif
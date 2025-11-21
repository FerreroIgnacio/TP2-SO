#ifndef STDOUT_H
#define STDOUT_H

#include <stdint.h>

// Tamaño del buffer de entrada estándar
#define STDOUTLEN 4096

// Inserta un carácter en el buffer
void queueKeyStdout(char c);

// Inserta un string al stdout
void queueStringStdout(const char* str);

// Extrae un carácter del buffer
char consumeKeyStdout(void);

// Devuelve 1 si hay datos disponibles, 0 si está vacío
int stdout_has_data(void);

#endif


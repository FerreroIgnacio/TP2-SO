#ifndef STDIN_H
#define STDIN_H

#include <stdint.h>

// Tamaño del buffer de entrada estándar
#define STDINLEN 4096

// Inserta un carácter en el buffer (llamado desde el ISR del teclado)
void queueKeyStdin(char c);

// Extrae un carácter del buffer (bloquea si el buffer está vacío)
char consumeKeyStdin(void);

// Devuelve 1 si hay datos disponibles, 0 si está vacío
int stdin_has_data(void);

#endif


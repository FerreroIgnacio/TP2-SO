#include <util.h>

// Convierte un entero en string, almacenado en buffer (mínimo 12 bytes).
char* stringFromInt(int value, char* buffer) {
    int i = 0;
    int isNegative = 0;

    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    if (value < 0) {
        isNegative = 1;
        value = -value;
    }

    // Extraer dígitos en orden inverso
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (isNegative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Invertir la cadena
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char tmp = buffer[j];
        buffer[j] = buffer[k];
        buffer[k] = tmp;
    }

    return buffer;
}


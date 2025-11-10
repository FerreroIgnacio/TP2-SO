#include "./keyboard.h"

/* FUNCIONALIDADES DEL TECLADO */
// 1 si las (count) teclas de (makecode) estás presionadas, 0 si no.
int areKeysPressed(int *makecodes, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (!isKeyPressed(makecodes[i]))
        {
            return 0;
        }
    }
    return 1;
}
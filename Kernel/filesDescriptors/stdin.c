#include "./stdin.h"

static char STDIN[STDINLEN];

int startIndexIn = 0;
int endIndexIn = 0;

char consumeKeyStdin()
{
    while (startIndexIn == endIndexIn)
    {
        // Esperar (polling o halt_cpu si querés usar interrupciones)
    }

    char c = STDIN[startIndexIn];
    startIndexIn = (startIndexIn + 1) % STDINLEN;
    return c;
}

void queueKeyStdin(char c)
{
    int nextEnd = (endIndexIn + 1) % STDINLEN;
    if (nextEnd == startIndexIn)
    {
        // Buffer lleno, descartamos o sobreescribimos (acá descartamos)
        return;
    }
    STDIN[endIndexIn] = c;
    endIndexIn = nextEnd;
}

int stdin_has_data()
{
    return startIndexIn != endIndexIn;
}

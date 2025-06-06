#include <stdout.h>

static char STDOUT[STDOUTLEN];

int startIndexOut = 0;
int endIndexOut = 0;



void queueKeyStdout(char c) {
    int nextEnd = (endIndexOut + 1) % STDOUTLEN;
    if (nextEnd == startIndexOut) {
        // Buffer lleno, descartamos o sobreescribimos (acá descartamos)
        return;
    }
    STDOUT[endIndexOut] = c;
    endIndexOut = nextEnd;
}

void queueStringStdout(const char * str){
    while (*str){
        queueKeyStdout(*str++);
    }
}

int stdout_has_data() {
    return startIndexOut != endIndexOut;
}

char consumeKeyStdout() {
    char c = STDOUT[startIndexOut];
    startIndexOut = (startIndexOut + 1) % STDOUTLEN;
    return c;
}
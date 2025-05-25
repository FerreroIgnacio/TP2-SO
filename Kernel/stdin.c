

#define STDINLEN 4096
static char STDIN[STDINLEN];

int startIndex = 0;
int endIndex = 0;

char consumeKey() {
    while (startIndex == endIndex) {
        // Esperar (polling o halt_cpu si querés usar interrupciones)
    }

    char c = STDIN[startIndex];
    startIndex = (startIndex + 1) % STDINLEN;
    return c;
}

int stdin_has_data() {
    return startIndex != endIndex;
}
void queueKey(char c) {
    int nextEnd = (endIndex + 1) % STDINLEN;
    if (nextEnd == startIndex) {
        // Buffer lleno, descartamos o sobreescribimos (acá descartamos)
        return;
    }
    STDIN[endIndex] = c;
    endIndex = nextEnd;
}





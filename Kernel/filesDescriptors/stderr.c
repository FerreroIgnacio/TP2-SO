#include "./stderr.h"

static char STDERR_BUF[STDERRLEN];
static int startIndexErr = 0;
static int endIndexErr = 0;

void queueKeyStderr(char c)
{
    int nextEnd = (endIndexErr + 1) % STDERRLEN;
    if (nextEnd == startIndexErr)
    {
        // Buffer lleno: descartar nuevo dato (mismo comportamiento que stdout)
        return;
    }
    STDERR_BUF[endIndexErr] = c;
    endIndexErr = nextEnd;
}

int stderr_has_data()
{
    return startIndexErr != endIndexErr;
}

char consumeKeyStderr(void)
{
    if (startIndexErr == endIndexErr)
    {
        return 0; // no blocking here; caller should poll like stdout non-blocking reads
    }
    char c = STDERR_BUF[startIndexErr];
    startIndexErr = (startIndexErr + 1) % STDERRLEN;
    return c;
}


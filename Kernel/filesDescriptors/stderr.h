#ifndef STDERR_H
#define STDERR_H

#include <stdint.h>

#define STDERRLEN 4096

void queueKeyStderr(char c);
char consumeKeyStderr(void);
int stderr_has_data(void);

#endif // STDERR_H


#ifndef PIPES_H
#define PIPES_H
#include <stdint.h>

#define MAX_PIPES 64
#define PIPE_BUFFER_CAPACITY 4096

// Simple kernel-level pipe (byte ring buffer)
int pipe_create(void);
int pipe_write(int pipe_id, const char *buffer, uint64_t count);
int pipe_read(int pipe_id, char *buffer, uint64_t count);

#endif // PIPES_H


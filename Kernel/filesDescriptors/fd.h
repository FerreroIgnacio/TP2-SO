#ifndef FD_H
#define FD_H
#include <stdint.h>

// Indice de inicio file descriptors (0=STDIN,1=STDOUT,2=STDERR reserved)
#define FIRST_DYNAMIC_FD 3
#define MAX_DYNAMIC_FDS 32
#define FD_NAME_MAX 32

// Simple byte buffer per FD
#define FD_BUFFER_CAPACITY 4096

typedef struct fd_entry {
    char name[FD_NAME_MAX];
    uint8_t buffer[FD_BUFFER_CAPACITY];
    uint32_t read_pos; // next position to read
    uint32_t write_pos; // next position to write
    uint32_t size; // current bytes stored
    uint8_t in_use;
} fd_entry_t;

// Public info for listing FDs
typedef struct fd_info {
    int fd;                      // absolute fd number
    char name[FD_NAME_MAX];      // null-terminated name
    uint32_t size;               // bytes queued
} fd_info_t;

// Initialize subsystem
void fd_init(void);
// Create a new FD with given name. Returns fd number (>= FIRST_DYNAMIC_FD) or -1.
int fd_create(const char *name);
// Write count bytes to fd, returns bytes written or -1 if error.
int fd_write(int fd, const char *buffer, uint64_t count);
// Read up to count bytes from fd into buffer, returns bytes read or -1 if error.
int fd_read(int fd, char *buffer, uint64_t count);
// Simple helper to check if fd has data (>0 bytes stored)
int fd_has_data(int fd);
// List active dynamic FDs into out (up to max). Returns count filled.
int fd_list(fd_info_t *out, int max);

#endif // FD_H

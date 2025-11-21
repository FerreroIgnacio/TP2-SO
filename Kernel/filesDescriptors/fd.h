#ifndef FD_H
#define FD_H
#include <stdint.h>

// Indice de inicio file descriptors (0=STDIN,1=STDOUT,2=STDERR reserved)
#define FIRST_DYNAMIC_FD 3
// Maximos FDs dinamicos por proceso
#define MAX_PROCESS_FDS 32
#define FD_NAME_MAX 32

// Simple byte buffer per FD (pipe-like)
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
    int fd;                      // per-process fd number
    char name[FD_NAME_MAX];      // null-terminated name
    uint32_t size;               // bytes queued
} fd_info_t;

// Initialize subsystem
void fd_init(void);
// Resetea la tabla de FDs del proceso pid (se invoca al crear el proceso)
void fd_reset_pid(int pid);
// Create a new FD with given name in current process. Returns fd number (>= FIRST_DYNAMIC_FD) or -1.
int fd_create(const char *name);
// Write count bytes to fd of current process, returns bytes written or -1 if error.
int fd_write(int fd, const char *buffer, uint64_t count);
// Read up to count bytes from fd of current process into buffer, returns bytes read or -1 if error.
int fd_read(int fd, char *buffer, uint64_t count);
// Simple helper to check if fd has data (>0 bytes stored) in current process
int fd_has_data(int fd);
// List active dynamic FDs for current process into out (up to max). Returns count filled.
int fd_list(fd_info_t *out, int max);

// Bind STDIN/STDOUT of a given pid to a pipe (which: 0=STDIN, 1=STDOUT). Returns 0 or -1.
int fd_bind_std_for_pid(int pid, int which, int pipe_id);
// Query bound pipes for current process; return pipe_id >= 0 or -1 if none
int fd_get_bound_stdin_pipe(void);
int fd_get_bound_stdout_pipe(void);

#endif // FD_H

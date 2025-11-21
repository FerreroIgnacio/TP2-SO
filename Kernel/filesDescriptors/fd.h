#ifndef FD_H
#define FD_H
#include <stdint.h>

// Macros de FDs estandar por convencion POSIX like
#ifndef STDIN
#define STDIN 0
#endif
#ifndef STDOUT
#define STDOUT 1
#endif
#ifndef STDERR
#define STDERR 2
#endif

// File descriptors por proceso empiezan en 0 (0=STDIN,1=STDOUT,2=STDERR)
#define FIRST_DYNAMIC_FD 0
#define MAX_PROCESS_FDS 32
#define FD_NAME_MAX 32
#define FD_BUFFER_CAPACITY 4096

/*

 * Semantica (PARCIAL):
 * - Cada proceso tiene su propia tabla de FDs, numerados desde 0.
 * - Los FDs 0,1,2 corresponden a stdin, stdout, stderr del proceso.
 * - fd_read y fd_write son BLOQUEANTES: intentan transferir exactamente 'count' bytes.
 *   Si el buffer del FD está vacío (read) o lleno (write), se cede CPU (scheduler_yield)
 *   y se reintenta hasta completar la operación.
 * - Si el FD está redirigido a una pipe (ver fd_bind_std_for_pid para stdin/stdout),
 *   la semántica de bloqueo es la de la pipe (también bloqueante).
 * - En caso de parámetros inválidos (FD inexistente/no en uso en el proceso, punteros nulos,
 *   count == 0), fd_read/fd_write retornan -1 y no transfieren datos.
 */

typedef struct fd_entry {
    char name[FD_NAME_MAX];
    uint8_t buffer[FD_BUFFER_CAPACITY];
    uint32_t read_pos;
    uint32_t write_pos;
    uint32_t size;
    uint8_t in_use;
} fd_entry_t;

// Public info for listing FDs
typedef struct fd_info {
    int fd;                      // per-process fd number
    char name[FD_NAME_MAX];      // null-terminated name
    uint32_t size;               // bytes queued
} fd_info_t;

// Initialize subsystem (resetea todas las tablas y crea FDs 0,1,2 por proceso). No retorna valor.
void fd_init(void);
// Resetea la tabla de FDs del proceso pid (se invoca al crear el proceso). No retorna valor.
void fd_reset_pid(int pid);
// Crea un nuevo FD con 'name' en el proceso actual.
// Retorno: número de FD (>= FIRST_DYNAMIC_FD) si OK; -1 si error/sin espacio/args inválidos.
int fd_create(const char *name);
// Escribe hasta 'count' bytes (parcial). Bloquea solo si no puede escribir ni 1 byte inicialmente.
int fd_write(int fd, const char *buffer, uint64_t count);
// Lee hasta 'count' bytes (parcial). Bloquea solo si no puede leer ni 1 byte inicialmente.
int fd_read(int fd, char *buffer, uint64_t count);
// Helper: devuelve 1 si el FD tiene datos (>0 bytes), 0 si no, -1 si FD inválido.
int fd_has_data(int fd);
// Lista FDs activos del proceso actual en 'out' (hasta 'max' entradas).
// Retorno: cantidad de elementos llenados (>=0).
int fd_list(fd_info_t *out, int max);

// Enlaza STDIN/STDOUT de 'pid' a una pipe (which: 0=STDIN, 1=STDOUT).
// A partir del enlace, fd_read/fd_write sobre ese FD usarán la pipe (bloqueante).
// Retorno: 0 si OK; -1 si error (pid inválido o which inválido).
int fd_bind_std_for_pid(int pid, int which, int pipe_id);

#endif // FD_H

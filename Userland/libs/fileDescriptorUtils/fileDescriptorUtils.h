#ifndef FILEDESCRIPTORUTILS_H
#define FILEDESCRIPTORUTILS_H

#include <stdint.h>
#include <stddef.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define FD_SIZE 4096

#define SYSCALL_POLL 74
#define SYSCALL_SELECT 75

#ifndef EOF
#define EOF (-1)
#endif

/* UTILIDADES FILE DESCRIPTORS */
// write: Escribe hasta 'count' bytes de 'buff' en el FD 'fd'.
// Retorna: cantidad efectiva escrita (0..count) o -1 si fd/args inválidos.
int write(int fd, const char *buff, unsigned long count);
// read: Lee hasta 'count' bytes desde el FD 'fd' hacia 'buff'.
// Retorna: cantidad efectiva leída (0..count) o -1 si fd/args inválidos.
int read(int fd, unsigned char *buff, unsigned long count);
// fd_open: Crea un nuevo FD dinámico con nombre (retrocompatibilidad).
// Retorna: fd>=3 si éxito, -1 si no hay espacio o args inválidos.
int     fd_open(const char *name);
// fd_list: Lista FDs dinámicos del proceso actual (hasta 'max').
// Retorna: cantidad de entradas copiadas (>=0).
typedef struct fd_info_u {
    int fd;        // número de fd absoluto
    char name[32]; // nombre (máx 31 chars + '\0')
    uint32_t size; // bytes en buffer
} fd_info_u_t;
int fd_list(fd_info_u_t *out, int max);          // Retorna cantidad listada (>=0)
// fd_list_pid: Lista FDs dinámicos del proceso 'pid'.
// Retorna: cantidad listada (>=0) o 0 si pid inválido / sin FDs.
int fd_list_pid(int pid, fd_info_u_t *out, int max);

// Pipes y redirección
// pipe_create: Crea una pipe y devuelve su id (>=0) o -1 si no hay espacio.
int pipe_create(void);

// fd_bind_std: Enlaza FD de 'pid' a 'pipe_id'.
// Retorna: 0 si ok, -1 si parámetros inválidos.
int fd_bind_std(int pid, int whichFD, int pipe_id);
// fd_has_data: Indica si el FD tiene datos listos para leer.
// Retorna: 1 si hay datos, 0 si vacío, -1 si fd inválido.
int fd_has_data(int fd);

// flush: Vacía todo el contenido pendiente del FD (descarta datos). No retorna valor.
void flush(int fd);

/* UTILIDADES STDIN */
// getchar: Bloquea hasta leer 1 byte de STDIN. Retorna el unsigned char leído.
unsigned char getchar();
// scanf: Parsea según 'format' y asigna en argumentos. Retorna cantidad de items asignados.
int scanf(const char *format, ...);

/* UTILIDADES STDOUT */
// putchar: Escribe un único carácter en STDOUT.
void putchar(char c);             // No retorna; siempre intenta escribir 1 byte.
// puts: Escribe la cadena 'str' (sin agregar newline adicional si ya existe). Retorna void.
void puts(const char *str);
// putuint: Imprime valor unsigned decimal. Retorna void.
void putuint(uint64_t c);
// putint: Imprime valor signed decimal (maneja signo). Retorna void.
void putint(int64_t c);
// putoct: Imprime valor en base 8. Retorna void.
void putoct(uint64_t c);
// puthex: Imprime valor en hexadecimal minúsculas. Retorna void.
void puthex(uint64_t c);
// puthexupper: Imprime valor en hexadecimal mayúsculas. Retorna void.
void puthexupper(uint64_t value);
// void putdouble(double value, int precision); //%f
// void putdouble(double value, int precision); //%f (no implementado)
/*
 * printf: imprime en STDOUT con formato estilo C básico.
 * Soporta: %c %s %d %u %o %x %X %p %P (con modificadores l / ll y '#' para 0x/0 prefix).
 * Retorna: cantidad de caracteres emitidos (bytes escritos) en condiciones normales.
 * Nota: la salida puede BLOQUEAR si el buffer asociado a STDOUT está lleno.
 * Retorna: cantidad de caracteres emitidos (>=0).
 */
uint64_t printf(const char *format, ...);
/*
 * fprintf: imprime en el file descriptor 'fd' con el mismo formato básico que printf.
 * Soporta: %c %s %d %u %o %x %X %p %P (con modificadores l / ll y '#' para 0x/0 prefix).
 * Retorna: cantidad de caracteres emitidos (bytes escritos) en condiciones normales.
 * Nota: la salida puede BLOQUEAR si el buffer del fd está lleno.
 * Retorna: cantidad de caracteres emitidos (>=0).
 */
uint64_t fprintf(int fd, const char *format, ...);

/* Estructura y API de userland para listar pipes del kernel */
typedef struct pipe_info_u {
    int id;
    int in_use;
    unsigned int size;       // bytes en buffer
    unsigned int capacity;   // capacidad total
    unsigned int readers_waiting; // 0/1
    unsigned int writers_waiting; // 0/1
} pipe_info_u_t;
// pipe_list_u: Lista hasta 'max' pipes en 'out'. Retorna cantidad copiada (>=0).
int pipe_list_u(pipe_info_u_t *out, int max);

#endif

#ifndef FILEDESCRIPTORUTILS_H
#define FILEDESCRIPTORUTILS_H

#include <stdint.h>
#include <stddef.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STD_BUFF_SIZE 4096

/* UTILIDADES FILE DESCRIPTORS */
// Escribir (count) caracteres de (buf) en el file descriptor (fd).
int write(int fd, const char *buff, unsigned long count);
// Leer (count) caracteres del file descriptor (fd) y guardar en (buf).
int read(int fd, unsigned char *buff, unsigned long count);
// Abrir/crear un FD dinámico con nombre; retorna fd>=2 o -1
int fd_open(const char *name);
// Listar FDs dinámicos: llena hasta max y retorna cantidad.
typedef struct fd_info_u
{
    int fd;        // número de fd absoluto
    char name[32]; // nombre (máx 31 chars + '\0')
    uint32_t size; // bytes en buffer
} fd_info_u_t;
int fd_list(fd_info_u_t *out, int max);

// Pipes y redirección
int pipe_create(void);                // crea un pipe y retorna id
int fd_bind_std(int pid, int which, int pipe_id); // which: 0=STDIN, 1=STDOUT

// borra todo el file descriptor
void flush(int fd);

/* UTILIDADES STDIN */
unsigned char getchar();
int scanf(const char *format, ...);

/* UTILIDADES STDOUT */
void putchar(char c);             // %c
void puts(const char *str);       // %s
void putuint(uint64_t c);         // %u
void putint(int64_t c);           // %d %ld %lld
void putoct(uint64_t c);          // %o
void puthex(uint64_t c);          // %x %p
void puthexupper(uint64_t value); // %X %P
/* Falta agregar soporte para double */
// void putdouble(double value, int precision); //%f
uint64_t printf(const char *format, ...);

#endif
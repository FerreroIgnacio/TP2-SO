#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "../fontManager/fontManager.h"

#define FONT_HEIGHT 8
#define FONT_WIDTH 8

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STD_BUFF_SIZE 4096
// Reduce from 100MB to 16MB to fit typical LFB sizes without exhausting memory
#define FRAMEBUFFER_SIZE 100000000

/* MANEJO DE STRINGS */
int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint64_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, uint64_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, uint64_t n);

int64_t strtoint(const char *str);
uint64_t strtouint(const char *str);
uint64_t strtohex(const char *str);
uint64_t strtooct(const char *str);
// genérico para uint en dec, hex y oct
uint64_t strtoint_complete(const char *str);

/* UTILIDADES FILE DESCRIPTORS */
// Escribir (count) caracteres de (buf) en el file descriptor (fd).
int write(int fd, const char *buff, unsigned long count);
// Leer (count) caracteres del file descriptor (fd) y guardar en (buf).
int read(int fd, unsigned char *buff, unsigned long count);

/* FUNCIONALIDADES DEL TECLADO */
// 1 si la tecla (makecode) está presionada, 0 si no.
// para scancodes especiales agregar E0, por ejemplo, 0xE048 para flecha arriba
int isKeyPressed(uint16_t makecode);
// 1 si las (count) teclas de (makecode) estás presionadas, 0 si no.
int areKeysPressed(int *makecodes, int count);

/* TIME */
// Tiempo de booteo en ms
uint64_t getBootTime();
// Guarda el tiempo según el reloj interno del dispositivo (por lo general, UTC 0)
void getLocalTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
// Guarda la fecha según el reloj interno del dispositivo (por lo general, UTC 0)
void getLocalDate(uint8_t *year, uint8_t *month, uint8_t *day);

/* CAPTURA DE REGISTROS */
// Se define struct registers para mejorar la sintaxis.
typedef struct registers
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t rip;
} registers_t;
// Guarda en regs la captura de los registros realizada con F1
void getRegisters(registers_t *regs);
// Alias solicitado: readregister() lee la captura de registros en el mismo formato
void readregister(registers_t *regs);

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

/* SONIDO */
void playFreq(uint16_t freq, uint64_t ms);
void startSound(uint16_t freq);
void stopSound();

uint32_t rand();

void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void getMemInfo(size_t *total, size_t *used, size_t *free);

#endif

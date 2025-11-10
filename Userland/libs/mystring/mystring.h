#ifndef MYSTRING_H
#define MYSTRING_H

#include <stdint.h>
#include <stddef.h>

/* MANEJO DE STRINGS */
size_t strlen(const char *str);
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

char *find_args(char *cmd);

#endif
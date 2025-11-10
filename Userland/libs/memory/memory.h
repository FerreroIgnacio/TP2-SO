#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include <stddef.h>

void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void getMemInfo(size_t *total, size_t *used, size_t *free);

#endif
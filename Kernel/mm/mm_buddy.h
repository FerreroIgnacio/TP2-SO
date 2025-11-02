#ifndef MM_BUDDY_H
#define MM_BUDDY_H


#include <stddef.h>
#include <stdint.h>


void buddy_init(void *heap_start, size_t heap_size);
void *buddy_malloc(size_t size);
void *buddy_calloc(size_t size);
void *buddy_realloc(void *ptr, size_t size);
void buddy_free(void *ptr);

#endif /* MM_BUDDY_H */
#ifndef MM_FREELIST_H
#define MM_FREELIST_H

#include <stddef.h>
#include <stdint.h>

void freelist_init(void *heap_start, size_t heap_size);
void *freelist_malloc(size_t size);
void *freelist_calloc(size_t count, size_t size);
void *freelist_realloc(void *ptr, size_t size);
void freelist_free(void *ptr);

size_t freelist_largest_free_block(void);
size_t freelist_total_free(void);

#endif

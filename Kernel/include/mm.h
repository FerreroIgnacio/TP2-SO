#ifndef MM_H
#define MM_H

#include <stddef.h>
#include <stdint.h>

/**
 * Initializes the memory manager with the provided heap region.
 *
 * @param heap_start Pointer to the beginning of the heap (must be aligned).
 * @param heap_size  Size in bytes of the heap region.
 */
void mm_init(void *heap_start, size_t heap_size);

void *mm_malloc(size_t size);
void *mm_calloc(size_t count, size_t size);
void *mm_realloc(void *ptr, size_t size);
void mm_free(void *ptr);

/* Convenience aliases that mirror the standard C API. */
void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

#endif /* MM_H */

#include "../include/mm.h"
#include "mm_internal.h"

#ifdef MM_BUDDY
#error "Buddy allocator is not implemented. Compile without MM_BUDDY."
#else

void mm_init(void *heap_start, size_t heap_size) {
    freelist_init(heap_start, heap_size);
}

void *mm_malloc(size_t size) {
    return freelist_malloc(size);
}

void *mm_calloc(size_t count, size_t size) {
    return freelist_calloc(count, size);
}

void *mm_realloc(void *ptr, size_t size) {
    return freelist_realloc(ptr, size);
}

void mm_free(void *ptr) {
    freelist_free(ptr);
}

void *malloc(size_t size) {
    return mm_malloc(size);
}

void *calloc(size_t count, size_t size) {
    return mm_calloc(count, size);
}

void *realloc(void *ptr, size_t size) {
    return mm_realloc(ptr, size);
}

void free(void *ptr) {
    mm_free(ptr);
}

#endif

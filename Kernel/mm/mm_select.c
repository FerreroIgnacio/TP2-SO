#include "../include/mm.h"
#include "mm_freelist.h"
#include "mm_buddy.h"

void mm_init(void *heap_start, size_t heap_size)
{
#ifdef USE_BUDDY
    buddy_init(heap_start, heap_size);
#else
    freelist_init(heap_start, heap_size);
#endif
}

void *mm_malloc(size_t size)
{
#ifdef USE_BUDDY
    return buddy_malloc(size);
#else
    return freelist_malloc(size);
#endif
}

void *mm_calloc(size_t count, size_t size)
{
#ifdef USE_BUDDY
    return buddy_calloc(size);
#else
    return freelist_calloc(count, size);
#endif
}

void *mm_realloc(void *ptr, size_t size)
{
#ifdef USE_BUDDY
    return buddy_realloc(ptr, size);
#else
    return freelist_realloc(ptr, size);
#endif
}

void mm_free(void *ptr)
{
#ifdef USE_BUDDY
    buddy_free(ptr);
#else
    freelist_free(ptr);
#endif
}

void mm_get_memory_info(size_t *total, size_t *used)
{
#ifdef USE_BUDDY
    buddy_get_memory_info(total, used);
#else
    freelist_get_memory_info(total, used);
#endif
}

void *malloc(size_t size)
{
    return mm_malloc(size);
}

void *calloc(size_t count, size_t size)
{
    return mm_calloc(count, size);
}

void *realloc(void *ptr, size_t size)
{
    return mm_realloc(ptr, size);
}

void free(void *ptr)
{
    mm_free(ptr);
}

void get_memory_info(size_t *total, size_t *used)
{
    mm_get_memory_info(total, used);
}

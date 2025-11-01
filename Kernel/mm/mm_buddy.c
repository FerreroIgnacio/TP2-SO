#include "mm_buddy.h"
#define MAX_ORDER 29 // (512 MB) = 2^29 bytes
#define MAX_BLOCK_SIZE (1UL << MAX_ORDER)

/* Buddy Memory Allocator Implementation
 * block_t funciona como metadata para cada bloque de memoria
 * el usuario de malloc recibe el puntero a la dirección siguiente del block_t
 *
 *
 */
typedef struct block
{
    struct block *next;
    struct block *prev;
    int order;
    int is_free;
} block_t;

void *heapStart = NULL;
size_t maxBlockSize = 0;
block_t *freeLists[MAX_ORDER + 1] = {NULL};
char memory[MAX_BLOCK_SIZE]

    static size_t
    calculateOrder(size_t size)
{
    size += sizeof(block_t);

    int order = 0;
    size_t blockSize = 1;
    while (blockSize < size)
    {
        blockSize <<= 1;
        order++;
    }
    return order;
}

void buddy_init(void *heap_start, size_t heap_size)
{
    size_t order = -1;
    heapStart = heap_start;
    while (maxBlockSize < heap_size)
    {
        maxBlockSize <<= 1;
        order++;
    }
    maxBlockSize >>= 1;
    freeLists[order] = (block_t *)heapStart;
}

void *buddy_malloc(size_t size)
{
    return NULL;
}
void *buddy_calloc(size_t count, size_t size)
{
    return NULL;
}
void *buddy_realloc(void *ptr, size_t size)
{
    return NULL;
}
void buddy_free(void *ptr)
{
}
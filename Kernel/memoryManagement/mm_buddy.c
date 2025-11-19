#include "mm_buddy.h"
#include <string.h>
#define MAX_ORDER 29 // (512 MB) = 2^29 bytes

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

block_t *freeLists[MAX_ORDER + 1] = {NULL};
size_t heap_order = 0;

static size_t total_memory = 0;
static size_t used_memory = 0;
static uint8_t *heap_base = NULL;
static size_t heap_block_size = 0; // bytes covered by the top-level block

static size_t calculate_order(size_t size)
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

static size_t floor_order(size_t size)
{
    // Largest order such that 2^order <= size
    size_t order = 0;
    size_t blockSize = 1;
    while ((blockSize << 1) <= size && order < MAX_ORDER)
    {
        blockSize <<= 1;
        order++;
    }
    return order;
}

static block_t *find_buddy(block_t *block)
{
    // Compute buddy relative to the heap base so the XOR stays inside the heap
    size_t offset = (size_t)((uint8_t *)block - heap_base);
    size_t buddy_offset = offset ^ (1UL << block->order);

    if (buddy_offset >= heap_block_size)
    {
        return NULL;
    }
    return (block_t *)(heap_base + buddy_offset);
}

static void insert_into_freelist(block_t *block)
{
    block->is_free = 1;
    block->next = freeLists[block->order];
    block->prev = NULL;
    if (freeLists[block->order])
    {
        freeLists[block->order]->prev = block;
    }
    freeLists[block->order] = block;
}

static void delete_from_freelist(block_t *block)
{
    if (block->prev)
    {
        block->prev->next = block->next;
    }
    else if (freeLists[block->order] == block)
    {
        freeLists[block->order] = block->next;
    }
    if (block->next)
    {
        block->next->prev = block->prev;
    }
}

void buddy_get_memory_info(size_t *total, size_t *used)
{
    if (total)
    {
        *total = total_memory;
    }
    if (used)
    {
        *used = used_memory;
    }
}

void buddy_init(void *heap_start, size_t heap_size)
{
    // Align the start to the block header size to avoid misaligned metadata
    uintptr_t aligned_start = ((uintptr_t)heap_start + (sizeof(block_t) - 1)) & ~(uintptr_t)(sizeof(block_t) - 1);
    if (heap_size <= aligned_start - (uintptr_t)heap_start)
    {
        return;
    }

    heap_base = (uint8_t *)aligned_start;
    heap_size -= aligned_start - (uintptr_t)heap_start;

    size_t order = floor_order(heap_size);
    if (order > MAX_ORDER)
    {
        order = MAX_ORDER;
    }
    heap_order = order;
    heap_block_size = (1UL << order);
    total_memory = heap_block_size;
    used_memory = 0;

    block_t *initial = (block_t *)heap_base;
    initial->order = order;
    initial->next = NULL;
    initial->prev = NULL;
    insert_into_freelist(initial);
}

void *buddy_malloc(size_t size)
{
    size_t order = calculate_order(size);
    for (int currentOrder = order; currentOrder <= MAX_ORDER; currentOrder++)
    {
        if (freeLists[currentOrder] != NULL)
        {
            block_t *block = freeLists[currentOrder];
            freeLists[currentOrder] = block->next;
            block->is_free = 0;
            block->next = NULL;
            while (block->order > order)
            {
                block_t *buddy = (block_t *)((size_t)block + (1UL << (block->order - 1)));
                buddy->order = block->order - 1;
                insert_into_freelist(buddy);
                block->order--;
            }
            used_memory += (1UL << block->order);
            return (uint8_t *)block + sizeof(block_t);
        }
    }
    return NULL;
}

void *buddy_calloc(size_t size)
{
    void *ptr = buddy_malloc(size);
    if (ptr)
    {
        size_t total_size = (1UL << calculate_order(size)) - sizeof(block_t);
        memset(ptr, 0, total_size);
        return ptr;
    }
    return NULL;
}

void *buddy_realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        return buddy_malloc(size);
    }

    size_t order = calculate_order(size);
    block_t *block = (block_t *)((uint8_t *)ptr - sizeof(block_t));
    if (block->order >= order)
    {
        return ptr;
    }
    for (int currentOrder = block->order; currentOrder <= order; currentOrder++)
    {
        block_t *buddy = find_buddy(block);
        if (buddy && buddy->is_free == 1 && buddy->order == block->order)
        {
            delete_from_freelist(buddy);
            if (block < buddy)
            {
                block->order++;
            }
            else
            {
                buddy->order++;
                buddy->is_free = 0;
                block = buddy;
            }
        }
        else
        {
            break;
        }
    }
    if (block->order >= order)
    {
        return ptr;
    }
    void *new_ptr = buddy_malloc(size);
    if (new_ptr)
    {
        size_t old_payload = (1UL << block->order) - sizeof(block_t);
        size_t new_payload = (1UL << calculate_order(size)) - sizeof(block_t);
        size_t copy_size = old_payload < new_payload ? old_payload : new_payload;
        memcpy(new_ptr, ptr, copy_size);
        buddy_free(ptr);
        return new_ptr;
    }
    return NULL;
}

void buddy_free(void *ptr)
{
    if (!ptr)
    {
        return;
    }
    block_t *block = (block_t *)((uint8_t *)ptr - sizeof(block_t));
    size_t block_size = (1UL << block->order);
    used_memory -= block_size;
    block->is_free = 1;

    while (block->order < heap_order)
    {
        block_t *buddy = find_buddy(block);
        if (!buddy || buddy->is_free != 1 || buddy->order != block->order) {
            break;
        }

        delete_from_freelist(buddy);
        if (block < buddy)
        {
            block->order++;
        }
        else
        {
            buddy->order++;
            buddy->is_free = 1;
            block = buddy;
        }
    }
    insert_into_freelist(block);
}

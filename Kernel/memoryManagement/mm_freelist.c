#include "mm_freelist.h"
#include "../utils/lib.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct block_header
{
    size_t size;               // Size of the payload area
    struct block_header *prev; // Previous free block in the free list
    struct block_header *next; // Next free block in the free list
    bool free;                 // Convenience flag to detect double free
} block_header_t;

#define ALIGNMENT 16UL
#define MIN_SPLIT_SIZE (sizeof(block_header_t) + ALIGNMENT)

static block_header_t *free_list_head = NULL;
static uint8_t *heap_begin = NULL;
static uint8_t *heap_end = NULL;

static size_t total_memory = 0;
static size_t used_memory = 0;

static size_t align_up(size_t size)
{
    if (size == 0)
    {
        return 0;
    }
    size_t remainder = size % ALIGNMENT;
    if (remainder == 0)
    {
        return size;
    }
    size_t aligned = size + (ALIGNMENT - remainder);
    if (aligned < size)
    { // overflow
        return 0;
    }
    return aligned;
}

static size_t align_down(size_t value)
{
    return value - (value % ALIGNMENT);
}

static bool pointer_in_heap(const void *ptr)
{
    return ptr != NULL && (const uint8_t *)ptr > heap_begin && (const uint8_t *)ptr <= heap_end;
}

static block_header_t *pointer_to_block(void *ptr)
{
    if (!pointer_in_heap(ptr))
    {
        return NULL;
    }
    return (block_header_t *)((uint8_t *)ptr - sizeof(block_header_t));
}

static void detach_block(block_header_t *block)
{
    if (block->prev)
    {
        block->prev->next = block->next;
    }
    else
    {
        free_list_head = block->next;
    }
    if (block->next)
    {
        block->next->prev = block->prev;
    }
    block->prev = NULL;
    block->next = NULL;
}

static block_header_t *merge_adjacent(block_header_t *left, block_header_t *right)
{
    if (!left || !right)
    {
        return left;
    }

    uint8_t *expected_address = (uint8_t *)left + sizeof(block_header_t) + left->size;
    if (expected_address != (uint8_t *)right)
    {
        return left;
    }

    left->size += sizeof(block_header_t) + right->size;
    left->next = right->next;
    if (right->next)
    {
        right->next->prev = left;
    }
    return left;
}

static void coalesce(block_header_t *block)
{
    // Merge with previous block if adjacent
    if (block->prev)
    {
        block = merge_adjacent(block->prev, block);
    }
    // Merge with next block if adjacent
    if (block->next)
    {
        merge_adjacent(block, block->next);
    }
}

static void insert_block(block_header_t *block)
{
    block->free = true;
    if (!free_list_head)
    {
        block->prev = block->next = NULL;
        free_list_head = block;
        return;
    }

    block_header_t *current = free_list_head;
    block_header_t *previous = NULL;

    while (current && current < block)
    {
        previous = current;
        current = current->next;
    }

    block->prev = previous;
    block->next = current;

    if (previous)
    {
        previous->next = block;
    }
    else
    {
        free_list_head = block;
    }

    if (current)
    {
        current->prev = block;
    }

    coalesce(block);
}

void freelist_init(void *heap_start, size_t heap_size)
{
    free_list_head = NULL;
    heap_begin = NULL;
    heap_end = NULL;

    if (!heap_start || heap_size < sizeof(block_header_t) + ALIGNMENT)
    {
        return;
    }

    uintptr_t start_addr = (uintptr_t)heap_start;
    uintptr_t aligned_start = (start_addr + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    size_t adjustment = aligned_start - start_addr;

    if (heap_size <= adjustment)
    {
        return;
    }

    heap_size -= adjustment;
    heap_size = align_down(heap_size);

    if (heap_size < sizeof(block_header_t) + ALIGNMENT)
    {
        return;
    }

    heap_begin = (uint8_t *)aligned_start;
    heap_end = heap_begin + heap_size;

    block_header_t *initial_block = (block_header_t *)heap_begin;
    initial_block->size = heap_size - sizeof(block_header_t);
    initial_block->prev = NULL;
    initial_block->next = NULL;
    initial_block->free = true;

    free_list_head = initial_block;
}

static block_header_t *find_first_fit(size_t aligned_size)
{
    block_header_t *current = free_list_head;
    while (current)
    {
        if (current->size >= aligned_size)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void freelist_get_memory_info(size_t *total, size_t *used)
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

void *freelist_malloc(size_t size)
{
    if (size == 0 || !free_list_head)
    {
        return NULL;
    }

    size_t aligned_size = align_up(size);
    if (aligned_size == 0)
    {
        return NULL;
    }

    block_header_t *block = find_first_fit(aligned_size);
    if (!block)
    {
        return NULL;
    }

    size_t remaining = block->size - aligned_size;

    if (remaining >= MIN_SPLIT_SIZE)
    {
        uint8_t *payload = (uint8_t *)block + sizeof(block_header_t);
        block_header_t *split_block = (block_header_t *)(payload + aligned_size);
        split_block->size = remaining - sizeof(block_header_t);
        split_block->free = true;
        split_block->prev = block->prev;
        split_block->next = block->next;

        if (split_block->next)
        {
            split_block->next->prev = split_block;
        }
        if (split_block->prev)
        {
            split_block->prev->next = split_block;
        }
        else
        {
            free_list_head = split_block;
        }

        block->size = aligned_size;
        block->prev = NULL;
        block->next = NULL;
        block->free = false;
        used_memory += aligned_size + sizeof(block_header_t);
        return payload;
    }

    detach_block(block);
    block->free = false;
    used_memory += block->size + sizeof(block_header_t);
    return (uint8_t *)block + sizeof(block_header_t);
}

void *freelist_calloc(size_t count, size_t size)
{
    if (count == 0 || size == 0)
    {
        return freelist_malloc(0);
    }

    if (count > SIZE_MAX / size)
    {
        return NULL;
    }

    size_t total = count * size;
    void *ptr = freelist_malloc(total);
    if (ptr)
    {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *freelist_realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        return freelist_malloc(size);
    }
    if (size == 0)
    {
        freelist_free(ptr);
        return NULL;
    }

    block_header_t *block = pointer_to_block(ptr);
    if (!block || block->free)
    {
        return NULL;
    }

    size_t aligned_size = align_up(size);
    if (aligned_size == 0)
    {
        return NULL;
    }

    if (aligned_size <= block->size)
    {
        size_t remaining = block->size - aligned_size;
        if (remaining >= MIN_SPLIT_SIZE)
        {
            block->size = aligned_size;
            uint8_t *payload = (uint8_t *)block + sizeof(block_header_t);
            block_header_t *new_block = (block_header_t *)(payload + aligned_size);
            new_block->size = remaining - sizeof(block_header_t);
            new_block->prev = NULL;
            new_block->next = NULL;
            new_block->free = true;
            used_memory -= new_block->size + sizeof(block_header_t);
            insert_block(new_block);
        }
        return ptr;
    }

    void *new_ptr = freelist_malloc(size);
    if (!new_ptr)
    {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    freelist_free(ptr);
    return new_ptr;
}

void freelist_free(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    block_header_t *block = pointer_to_block(ptr);
    if (!block || block->free)
    {
        return;
    }
    used_memory -= block->size + sizeof(block_header_t);
    insert_block(block);
}

size_t freelist_largest_free_block(void)
{
    size_t largest = 0;
    block_header_t *current = free_list_head;
    while (current)
    {
        if (current->size > largest)
        {
            largest = current->size;
        }
        current = current->next;
    }
    return largest;
}

size_t freelist_total_free(void)
{
    size_t total = 0;
    block_header_t *current = free_list_head;
    while (current)
    {
        total += current->size;
        current = current->next;
    }
    return total;
}
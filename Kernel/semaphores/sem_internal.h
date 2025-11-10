// Internal definitions for semaphore implementation.
#ifndef KERNEL_SYNC_SEM_INTERNAL_H
#define KERNEL_SYNC_SEM_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_SEMAPHORES 64
#define SEM_NAME_MAX 32

typedef struct spinlock {
    volatile int lock;
} spinlock_t;

void cpu_relax(void);

static inline void spinlock_init(spinlock_t *lock) {
    if (lock != NULL) {
        lock->lock = 0;
    }
}

static inline void spinlock_lock(spinlock_t *lock) {
    while (__atomic_test_and_set(&lock->lock, __ATOMIC_ACQUIRE)) {
        cpu_relax();
    }
}

static inline void spinlock_unlock(spinlock_t *lock) {
    __atomic_clear(&lock->lock, __ATOMIC_RELEASE);
}

typedef struct wait_node {
    int pid;
    struct wait_node *next;
    int status;
} wait_node_t;

typedef struct semaphore {
    char name[SEM_NAME_MAX];
    int64_t value;
    bool in_use;
    uint32_t ref_count;
    wait_node_t *waiters_head;
    wait_node_t *waiters_tail;
    spinlock_t lock;
} semaphore_t;

#endif // KERNEL_SYNC_SEM_INTERNAL_H

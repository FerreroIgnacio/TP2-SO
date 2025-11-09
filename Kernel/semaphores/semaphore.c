#include <stddef.h>
#include <stdbool.h>

#include "../utils/lib.h"
#include "sem.h"
#include "sem_internal.h"
#include "../scheduler/scheduler.h"
#include "../memoryManagement/mm.h"

static semaphore_t semaphores[MAX_SEMAPHORES];
static spinlock_t allocator_lock = {0};
static bool allocator_lock_init = false;

static void ensure_allocator_lock(void)
{
    if (!allocator_lock_init)
    {
        spinlock_init(&allocator_lock);
        allocator_lock_init = true;
    }
}

static semaphore_t *get_semaphore(int sem_id)
{
    if (sem_id < 0 || sem_id >= MAX_SEMAPHORES)
    {
        return NULL;
    }
    if (!semaphores[sem_id].in_use)
    {
        return NULL;
    }
    return &semaphores[sem_id];
}

static int semaphore_index(semaphore_t *sem)
{
    if (sem == NULL)
    {
        return -1;
    }
    return (int)(sem - semaphores);
}

static bool name_matches(const char *lhs, const char *rhs)
{
    if (lhs == NULL || rhs == NULL)
    {
        return false;
    }
    for (int i = 0; i < SEM_NAME_MAX; i++)
    {
        char a = lhs[i];
        char b = rhs[i];
        if (a != b)
        {
            return false;
        }
        if (a == '\0')
        {
            return true;
        }
    }
    return true;
}

static void copy_name(char dest[SEM_NAME_MAX], const char *src)
{
    if (dest == NULL)
    {
        return;
    }
    if (src == NULL)
    {
        dest[0] = '\0';
        return;
    }
    int i = 0;
    for (; i < SEM_NAME_MAX - 1 && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

static semaphore_t *find_by_name(const char *name)
{
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        if (semaphores[i].in_use && name_matches(semaphores[i].name, name))
        {
            return &semaphores[i];
        }
    }
    return NULL;
}

static semaphore_t *reserve_slot(const char *name, int64_t initial_value)
{
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        if (!semaphores[i].in_use)
        {
            semaphore_t *sem = &semaphores[i];
            memset(sem, 0, sizeof(*sem));
            sem->in_use = true;
            sem->value = initial_value;
            sem->ref_count = 1;
            copy_name(sem->name, name);
            spinlock_init(&sem->lock);
            return sem;
        }
    }
    return NULL;
}

static void release_waiters(wait_node_t *waiters, int status)
{
    wait_node_t *node = waiters;
    while (node != NULL)
    {
        wait_node_t *next = node->next;
        node->status = status;
        scheduler_unblock(node->pid, node, status);
        node = next;
    }
}

int sem_open(const char *name, int initial_value)
{
    if (name == NULL || name[0] == '\0')
    {
        return -1;
    }
    if (initial_value < 0)
    {
        return -1;
    }

    ensure_allocator_lock();
    spinlock_lock(&allocator_lock);

    semaphore_t *existing = find_by_name(name);
    if (existing != NULL)
    {
        spinlock_lock(&existing->lock);
        existing->ref_count++;
        spinlock_unlock(&existing->lock);
        int idx = semaphore_index(existing);
        spinlock_unlock(&allocator_lock);
        return idx;
    }

    semaphore_t *slot = reserve_slot(name, initial_value);
    if (slot == NULL)
    {
        spinlock_unlock(&allocator_lock);
        return -1;
    }

    int idx = semaphore_index(slot);
    spinlock_unlock(&allocator_lock);
    return idx;
}

int sem_wait(int sem_id)
{
    semaphore_t *sem = get_semaphore(sem_id);
    if (sem == NULL)
    {
        return -1;
    }

    int current_pid = scheduler_current_pid();
    if (current_pid < 0)
    {
        return -1;
    }

    wait_node_t *node_to_block = NULL;
    spinlock_lock(&sem->lock);

    if (!sem->in_use)
    {
        spinlock_unlock(&sem->lock);
        return -1;
    }

    if (sem->value > 0)
    {
        sem->value--;
        spinlock_unlock(&sem->lock);
        return 0;
    }

    node_to_block = (wait_node_t *)mm_malloc(sizeof(wait_node_t));
    if (node_to_block == NULL)
    {
        spinlock_unlock(&sem->lock);
        return -1;
    }

    node_to_block->pid = current_pid;
    node_to_block->next = NULL;
    node_to_block->status = 0;

    if (sem->waiters_tail != NULL)
    {
        sem->waiters_tail->next = node_to_block;
        sem->waiters_tail = node_to_block;
    }
    else
    {
        sem->waiters_head = sem->waiters_tail = node_to_block;
    }

    spinlock_unlock(&sem->lock);

    int status = scheduler_block_current(node_to_block);
    int result = (status == 0) ? 0 : -1;
    mm_free(node_to_block);
    return result;
}

int sem_post(int sem_id)
{
    semaphore_t *sem = get_semaphore(sem_id);
    if (sem == NULL)
    {
        return -1;
    }

    wait_node_t *node = NULL;

    spinlock_lock(&sem->lock);
    if (!sem->in_use)
    {
        spinlock_unlock(&sem->lock);
        return -1;
    }

    if (sem->waiters_head != NULL)
    {
        node = sem->waiters_head;
        sem->waiters_head = node->next;
        if (sem->waiters_head == NULL)
        {
            sem->waiters_tail = NULL;
        }
        node->next = NULL;
    }
    else
    {
        sem->value++;
    }

    spinlock_unlock(&sem->lock);

    if (node != NULL)
    {
        node->status = 0;
        scheduler_unblock(node->pid, node, 0);
    }

    return 0;
}

int sem_close(int sem_id)
{
    ensure_allocator_lock();
    spinlock_lock(&allocator_lock);

    semaphore_t *sem = get_semaphore(sem_id);
    if (sem == NULL)
    {
        spinlock_unlock(&allocator_lock);
        return -1;
    }

    wait_node_t *orphaned = NULL;
    bool reclaim = false;

    spinlock_lock(&sem->lock);
    if (sem->ref_count == 0)
    {
        spinlock_unlock(&sem->lock);
        spinlock_unlock(&allocator_lock);
        return -1;
    }

    sem->ref_count--;
    if (sem->ref_count == 0)
    {
        orphaned = sem->waiters_head;
        sem->waiters_head = NULL;
        sem->waiters_tail = NULL;
        sem->in_use = false;
        sem->name[0] = '\0';
        sem->value = 0;
        reclaim = true;
    }

    spinlock_unlock(&sem->lock);
    if (reclaim)
    {
        memset(sem, 0, sizeof(*sem));
    }
    spinlock_unlock(&allocator_lock);

    if (orphaned != NULL)
    {
        release_waiters(orphaned, -1);
    }

    return 0;
}

void sem_set(int sem_id, int value)
{
    semaphores[sem_id].value = value;
}

void sem_cleanup_dead_process(int pid)
{
    if (pid < 0)
    {
        return;
    }

    ensure_allocator_lock();
    spinlock_lock(&allocator_lock);

    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        semaphore_t *sem = &semaphores[i];
        if (!sem->in_use)
        {
            continue;
        }

        spinlock_lock(&sem->lock);

        wait_node_t *prev = NULL;
        wait_node_t *node = sem->waiters_head;

        while (node != NULL)
        {
            if (node->pid == pid)
            {
                wait_node_t *to_remove = node;
                if (prev == NULL)
                {
                    sem->waiters_head = node->next;
                }
                else
                {
                    prev->next = node->next;
                }

                if (sem->waiters_tail == node)
                {
                    sem->waiters_tail = prev;
                }

                node = node->next;

                mm_free(to_remove);
                continue;
            }

            prev = node;
            node = node->next;
        }

        spinlock_unlock(&sem->lock);
    }

    spinlock_unlock(&allocator_lock);
}

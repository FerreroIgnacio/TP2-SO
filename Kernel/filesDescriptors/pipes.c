#include "pipes.h"
#include "../scheduler/scheduler.h"     // for scheduler_block_current / scheduler_unblock
#include "../semaphores/sem_internal.h" // wait_node_t & spinlock
#include "../memoryManagement/mm.h"     // mm_malloc/mm_free

typedef struct {
    char buf[PIPE_BUFFER_CAPACITY];
    unsigned int rpos;
    unsigned int wpos;
    unsigned int size;
    int in_use;
    // Simplificado: 1 lector y 1 escritor por pipe
    int reader_pid;                 // -1 si no asignado
    int writer_pid;                 // -1 si no asignado
    wait_node_t *reader_waiter;     // lector bloqueado (si alguno)
    wait_node_t *writer_waiter;     // escritor bloqueado (si alguno)
    spinlock_t lock;
} pipe_t;

static pipe_t pipes[MAX_PIPES];

static inline int valid(int id) { return id >= 0 && id < MAX_PIPES && pipes[id].in_use; }

int pipe_create(void) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipes[i].in_use) {
            pipes[i].in_use = 1;
            pipes[i].rpos = pipes[i].wpos = pipes[i].size = 0;
            pipes[i].reader_pid = -1;
            pipes[i].writer_pid = -1;
            pipes[i].reader_waiter = NULL;
            pipes[i].writer_waiter = NULL;
            spinlock_init(&pipes[i].lock);
            return i;
        }
    }

    return -1;
}

int pipe_write(int id, const char *buffer, uint64_t count) {
    if (!valid(id) || buffer == 0 || count == 0) return -1;

    // Pipe 0 es solo escritura desde kernel (teclado) via pipe_try_kernel_nonblocking_write
    if (id == 0) return -1;

    pipe_t *p = &pipes[id];
    int me = scheduler_current_pid();
    uint64_t w = 0;

    while (w < count) {
        spinlock_lock(&p->lock);
        // Enforce single writer
        if (p->writer_pid == -1) {
            p->writer_pid = me;
        } else if (p->writer_pid != me) {
            // Otro writer intentando usar la misma pipe
            spinlock_unlock(&p->lock);
            return -1;
        }

        if (p->size == PIPE_BUFFER_CAPACITY) {
            // buffer lleno: si ya escribimos algo, devolver parcial; si no, bloquear
            if (w > 0) {
                spinlock_unlock(&p->lock);
                break;
            }
            // bloquear único writer
            if (p->writer_waiter == NULL) {
                wait_node_t *wn = (wait_node_t *)mm_malloc(sizeof(wait_node_t));
                if (!wn) { spinlock_unlock(&p->lock); return -1; }
                wn->pid = me; wn->status = 0; wn->next = NULL;
                p->writer_waiter = wn;
            }
            wait_node_t *my_node = p->writer_waiter;
            spinlock_unlock(&p->lock);
            int st = scheduler_block_current(my_node);
            (void)st;
            // el nodo es liberado por quien despierta? aquí lo liberamos tras despertar
            spinlock_lock(&p->lock);
            if (p->writer_waiter == my_node) { p->writer_waiter = NULL; }
            spinlock_unlock(&p->lock);
            mm_free(my_node);
            continue; // reintentar
        }

        // Hay espacio: escribir chunk
        unsigned int free_space = PIPE_BUFFER_CAPACITY - p->size;
        uint64_t remaining = count - w;
        uint64_t chunk = (remaining < free_space) ? remaining : free_space;
        int was_empty_before = (p->size == 0);
        for (uint64_t i = 0; i < chunk; i++) {
            p->buf[p->wpos] = buffer[w + i];
            p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
        }
        p->size += (unsigned int)chunk;
        if (was_empty_before && p->reader_waiter) {
            // despertar único lector
            wait_node_t *rd = p->reader_waiter;
            p->reader_waiter = NULL;
            scheduler_unblock(rd->pid, rd, 0);
        }
        spinlock_unlock(&p->lock);
        w += chunk;
    }
    return (int)w;
}

int pipe_read(int id, char *buffer, uint64_t count) {
    if (!valid(id) || buffer == 0 || count == 0) return -1;

    pipe_t *p = &pipes[id];
    int me = scheduler_current_pid();
    uint64_t r = 0;

    while (r < count) {
        spinlock_lock(&p->lock);
        // Enforce single reader
        if (p->reader_pid == -1) {
            p->reader_pid = me;
        } else if (p->reader_pid != me) {
            spinlock_unlock(&p->lock);
            return -1;
        }

        if (p->size == 0) {
            // vacío: parcial si ya leímos; sino bloquear
            if (r > 0) {
                spinlock_unlock(&p->lock);
                break;
            }
            if (p->reader_waiter == NULL) {
                wait_node_t *wn = (wait_node_t *)mm_malloc(sizeof(wait_node_t));
                if (!wn) { spinlock_unlock(&p->lock); return -1; }
                wn->pid = me; wn->status = 0; wn->next = NULL;
                p->reader_waiter = wn;
            }
            wait_node_t *my_node = p->reader_waiter;
            spinlock_unlock(&p->lock);
            int st = scheduler_block_current(my_node);
            (void)st;
            spinlock_lock(&p->lock);
            if (p->reader_waiter == my_node) { p->reader_waiter = NULL; }
            spinlock_unlock(&p->lock);
            mm_free(my_node);
            continue; // reintentar
        }

        // Hay datos: leer chunk
        uint64_t remaining = count - r;
        uint64_t available = p->size;
        uint64_t chunk = (remaining < available) ? remaining : available;
        int was_full_before = (p->size == PIPE_BUFFER_CAPACITY);
        for (uint64_t i = 0; i < chunk; i++) {
            buffer[r + i] = p->buf[p->rpos];
            p->rpos = (p->rpos + 1) % PIPE_BUFFER_CAPACITY;
        }
        p->size -= (unsigned int)chunk;
        if (was_full_before && p->writer_waiter) {
            // despertar único escritor
            wait_node_t *wr = p->writer_waiter;
            p->writer_waiter = NULL;
            scheduler_unblock(wr->pid, wr, 0);
        }
        spinlock_unlock(&p->lock);
        r += chunk;
    }
    return (int)r;
}

int pipe_try_kernel_nonblocking_write(int id, char c) {
    if (!valid(id)) return -1;

    pipe_t *p = &pipes[id];
    spinlock_lock(&p->lock);
    if (p->size == PIPE_BUFFER_CAPACITY) { spinlock_unlock(&p->lock); return 0; }
    int was_empty = (p->size == 0);
    p->buf[p->wpos] = c;
    p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
    p->size++;
    if (was_empty && p->reader_waiter) {
        wait_node_t *rd = p->reader_waiter;
        p->reader_waiter = NULL;
        scheduler_unblock(rd->pid, rd, 0);
    }
    spinlock_unlock(&p->lock);
    return 1;
}

int pipe_available(int id) {
    if (!valid(id)) return -1;
    pipe_t *p = &pipes[id];
    spinlock_lock(&p->lock);
    int sz = (int)p->size;
    spinlock_unlock(&p->lock);
    return sz;
}

int pipe_list(pipe_info_t *out, int max) {
    if (!out || max <= 0) return -1;
    int copied = 0;
    for (int i = 0; i < MAX_PIPES && copied < max; i++) {
        pipe_t *p = &pipes[i];
        int in_use = p->in_use;
        spinlock_lock(&p->lock);
        out[copied].id = i;
        out[copied].in_use = in_use;
        out[copied].size = p->size;
        out[copied].capacity = PIPE_BUFFER_CAPACITY;
        out[copied].readers_waiting = (p->reader_waiter != NULL) ? 1 : 0;
        out[copied].writers_waiting = (p->writer_waiter != NULL) ? 1 : 0;
        spinlock_unlock(&p->lock);
        copied++;
    }
    return copied;
}
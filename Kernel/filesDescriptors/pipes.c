#include "pipes.h"
#include "../scheduler/scheduler.h"     // for scheduler_block_current / scheduler_unblock
#include "../semaphores/sem_internal.h" // wait_node_t
#include "../memoryManagement/mm.h"     // mm_malloc/mm_free

typedef struct
{
    char buf[PIPE_BUFFER_CAPACITY];
    unsigned int rpos;
    unsigned int wpos;
    unsigned int size;
    int in_use;
    // Simplified for 1 reader and 1 writer: keep a single waiter per side
    wait_node_t *reader_waiter;
    wait_node_t *writer_waiter;
    // lock removed for single-threaded environment
} pipe_t;

static pipe_t pipes[MAX_PIPES];

static inline int valid(int id) { return id >= 0 && id < MAX_PIPES && pipes[id].in_use; }

int pipe_create(void)
{
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (!pipes[i].in_use)
        {
            for (int j = 0; j < PIPE_BUFFER_CAPACITY; j++)
            {
                pipes[i].buf[j] = 0;
            }
            pipes[i].in_use = 1;
            pipes[i].rpos = pipes[i].wpos = pipes[i].size = 0;
            pipes[i].reader_waiter = NULL;
            pipes[i].writer_waiter = NULL;
            // spinlock_init removed
            return i;
        }
    }
    return -1;
}

int pipe_write(int id, const char *buffer, uint64_t count)
{
    if (!valid(id) || buffer == 0 || count == 0)
        return -1;
    pipe_t *p = &pipes[id];
    uint64_t w = 0;
    while (w < count)
    {
        if (p->size == PIPE_BUFFER_CAPACITY)
        {
            // Buffer lleno. Si ya escribimos algo, devolvemos parcial sin bloquear.
            if (w > 0)
            {
                break;
            }
            // Nada escrito aún: bloquear hasta que haya espacio.
            if (p->writer_waiter == NULL)
            {
                wait_node_t *wn = (wait_node_t *)mm_malloc(sizeof(wait_node_t));
                if (!wn)
                {
                    return -1;
                }
                wn->pid = scheduler_current_pid();
                wn->status = 0;
                wn->next = NULL;
                p->writer_waiter = wn;
                scheduler_block_current(wn);
                // The waker keeps the pointer; we free it here once resumed.
                mm_free(wn);
                // Reintentar
                continue;
            }
            // Already a writer waiting (should not happen in 1 writer model), yield and retry.
            scheduler_yield();
            continue;
        }
        // Hay espacio para al menos 1 byte: escribir un bloque.
        unsigned int free_space = PIPE_BUFFER_CAPACITY - p->size;
        uint64_t remaining = count - w;
        uint64_t chunk = (remaining < free_space) ? remaining : free_space;
        int was_empty_before = (p->size == 0);
        for (uint64_t i = 0; i < chunk; i++)
        {
            p->buf[p->wpos] = buffer[w + i];
            p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
        }
        p->size += (unsigned int)chunk;
        // Notificar lector bloqueado si había vacío previamente
        if (was_empty_before && p->reader_waiter)
        {
            wait_node_t *rd = p->reader_waiter;
            p->reader_waiter = NULL;
            scheduler_unblock(rd->pid, rd, 0);
        }
        w += chunk;
        // Si se llenó y aún quedan datos, salimos (parcial).
    }
    return (int)w; // parcial o completo
}

int pipe_read(int id, char *buffer, uint64_t count)
{
    if (!valid(id) || buffer == 0 || count == 0)
        return -1;
    pipe_t *p = &pipes[id];
    uint64_t r = 0;
    while (r < count)
    {
        if (p->size == 0)
        {
            // Vacío. Si ya leímos algo, devolver parcial; si no, bloquear.
            if (r > 0)
            {
                break;
            }
            if (p->reader_waiter == NULL)
            {
                wait_node_t *wn = (wait_node_t *)mm_malloc(sizeof(wait_node_t));
                if (!wn)
                {
                    return -1;
                }
                wn->pid = scheduler_current_pid();
                wn->status = 0;
                wn->next = NULL;
                p->reader_waiter = wn;
                scheduler_block_current(wn);
                mm_free(wn);
                continue; // reintentar
            }
            // Already a reader waiting (should not happen in 1 reader model), yield and retry.
            scheduler_yield();
            continue;
        }
        // Hay datos: leer bloque.
        uint64_t remaining = count - r;
        uint64_t available = p->size;
        uint64_t chunk = (remaining < available) ? remaining : available;
        int was_full_before = (p->size == PIPE_BUFFER_CAPACITY);
        for (uint64_t i = 0; i < chunk; i++)
        {
            buffer[r + i] = p->buf[p->rpos];
            p->rpos = (p->rpos + 1) % PIPE_BUFFER_CAPACITY;
        }
        p->size -= (unsigned int)chunk;
        // Notificar escritor bloqueado si había lleno previamente
        if (was_full_before && p->writer_waiter)
        {
            wait_node_t *wr = p->writer_waiter;
            p->writer_waiter = NULL;
            scheduler_unblock(wr->pid, wr, 0);
        }
        r += chunk;
    }
    return (int)r; // parcial o completo
}

int pipe_try_kernel_nonblocking_write(int id, char c)
{
    if (!valid(id))
        return -1;
    pipe_t *p = &pipes[id];
    if (p->size == PIPE_BUFFER_CAPACITY)
    {
        return 0;
    }
    int was_empty = (p->size == 0);
    p->buf[p->wpos] = c;
    p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
    p->size++;
    if (was_empty && p->reader_waiter)
    {
        wait_node_t *rd = p->reader_waiter;
        p->reader_waiter = NULL;
        scheduler_unblock(rd->pid, rd, 0);
    }
    return 1;
}

int pipe_available(int id)
{
    if (!valid(id))
        return -1;
    pipe_t *p = &pipes[id];
    // Lectura concurrente segura: tamaño leído
    int sz = (int)p->size;
    return sz;
}

int pflush(int id)
{
    if (valid(id))
    {
        pipes[id].rpos = 0;
        pipes[id].wpos = 0;
        pipes[id].size = 0;
        return 0;
    }
    return -1;
}
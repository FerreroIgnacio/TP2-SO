#include "pipes.h"
#include "../scheduler/scheduler.h" // for scheduler_block_current / scheduler_unblock
#include "../semaphores/sem_internal.h" // wait_node_t & spinlock
#include "../memoryManagement/mm.h" // mm_malloc/mm_free
typedef struct {
    char buf[PIPE_BUFFER_CAPACITY];
    unsigned int rpos;
    unsigned int wpos;
    unsigned int size;
    int in_use;
    // NUEVO: listas de espera
    wait_node_t *readers_head;
    wait_node_t *readers_tail;
    wait_node_t *writers_head;
    wait_node_t *writers_tail;
    spinlock_t lock;
} pipe_t;
static pipe_t pipes[MAX_PIPES];

static inline int valid(int id){ return id >= 0 && id < MAX_PIPES && pipes[id].in_use; }

int pipe_create(void){
    for(int i=0;i<MAX_PIPES;i++){
        if(!pipes[i].in_use){
            pipes[i].in_use = 1;
            pipes[i].rpos = pipes[i].wpos = pipes[i].size = 0;
            pipes[i].readers_head = pipes[i].readers_tail = NULL;
            pipes[i].writers_head = pipes[i].writers_tail = NULL;
            spinlock_init(&pipes[i].lock);
            return i;
        }
    }
    return -1;
}

// Helper enqueue/dequeue
static void enqueue_waiter(wait_node_t **head, wait_node_t **tail, wait_node_t *node){
    node->next = NULL;
    if(*tail){ (*tail)->next = node; *tail = node; }
    else { *head = *tail = node; }
}
static wait_node_t *dequeue_waiter(wait_node_t **head, wait_node_t **tail){
    wait_node_t *n = *head; if(!n) return NULL; *head = n->next; if(*head==NULL) *tail=NULL; n->next=NULL; return n; }

int pipe_write(int id, const char *buffer, uint64_t count){
    if(!valid(id) || buffer==0 || count==0) return -1;
    pipe_t *p = &pipes[id];
    uint64_t w = 0;
    while(w < count){
        spinlock_lock(&p->lock);
        while(p->size == PIPE_BUFFER_CAPACITY){
            // Bloquear escritor actual
            wait_node_t *wn = (wait_node_t*)mm_malloc(sizeof(wait_node_t));
            if(!wn){ spinlock_unlock(&p->lock); return -1; }
            wn->pid = scheduler_current_pid(); wn->status = 0; wn->next = NULL;
            enqueue_waiter(&p->writers_head, &p->writers_tail, wn);
            spinlock_unlock(&p->lock);
            int st = scheduler_block_current(wn); // bloquea hasta wake
            mm_free(wn);
            // Reintentar: tomar lock nuevamente
            spinlock_lock(&p->lock);
        }
        // hay espacio
        p->buf[p->wpos] = buffer[w];
        p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
        int was_empty = (p->size == 0);
        p->size++;
        // Si había lectores esperando y la pipe estaba vacía, despertar uno
        if(was_empty && p->readers_head){
            wait_node_t *rd = dequeue_waiter(&p->readers_head, &p->readers_tail);
            // Desbloquear lector
            scheduler_unblock(rd->pid, rd, 0);
        }
        spinlock_unlock(&p->lock);
        w++;
    }
    return (int)w; // siempre count
}

int pipe_read(int id, char *buffer, uint64_t count){
    if(!valid(id) || buffer==0 || count==0) return -1;
    pipe_t *p = &pipes[id];
    uint64_t r = 0;
    while(r < count){
        spinlock_lock(&p->lock);
        while(p->size == 0){
            // Bloquear lector actual
            wait_node_t *wn = (wait_node_t*)mm_malloc(sizeof(wait_node_t));
            if(!wn){ spinlock_unlock(&p->lock); return -1; }
            wn->pid = scheduler_current_pid(); wn->status = 0; wn->next = NULL;
            enqueue_waiter(&p->readers_head, &p->readers_tail, wn);
            spinlock_unlock(&p->lock);
            int st = scheduler_block_current(wn);
            mm_free(wn);
            spinlock_lock(&p->lock);
        }
        // hay datos
        buffer[r] = p->buf[p->rpos];
        p->rpos = (p->rpos + 1) % PIPE_BUFFER_CAPACITY;
        int was_full = (p->size == PIPE_BUFFER_CAPACITY);
        p->size--;
        // Si había escritores esperando y estaba llena, despertar uno
        if(was_full && p->writers_head){
            wait_node_t *wr = dequeue_waiter(&p->writers_head, &p->writers_tail);
            scheduler_unblock(wr->pid, wr, 0);
        }
        spinlock_unlock(&p->lock);
        r++;
    }
    return (int)r; // siempre count
}

    int pipe_try_kernel_nonblocking_write(int id, char c){
    if(!valid(id)) return -1;
    pipe_t *p = &pipes[id];
    spinlock_lock(&p->lock);
    if(p->size == PIPE_BUFFER_CAPACITY){ spinlock_unlock(&p->lock); return 0; }
    int was_empty = (p->size == 0);
    p->buf[p->wpos] = c;
    p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
    p->size++;
    if(was_empty && p->readers_head){
        wait_node_t *rd = dequeue_waiter(&p->readers_head, &p->readers_tail);
        scheduler_unblock(rd->pid, rd, 0);
    }
    spinlock_unlock(&p->lock);
    return 1;
}

int pipe_available(int id){
    if(!valid(id)) return -1;
    pipe_t *p = &pipes[id];
    // Lectura concurrente segura: tamaño leído bajo lock
    spinlock_lock(&p->lock);
    int sz = (int)p->size;
    spinlock_unlock(&p->lock);
    return sz;
}

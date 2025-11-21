#include "pipes.h"
#include <string.h>

typedef struct {
    char buf[PIPE_BUFFER_CAPACITY];
    unsigned int rpos;
    unsigned int wpos;
    unsigned int size;
    int in_use;
} pipe_t;

static pipe_t pipes[MAX_PIPES];

static inline int valid(int id){ return id >= 0 && id < MAX_PIPES && pipes[id].in_use; }

int pipe_create(void){
    for(int i=0;i<MAX_PIPES;i++){
        if(!pipes[i].in_use){
            pipes[i].in_use=    1;
            pipes[i].rpos=pipes[i].wpos=pipes[i].size=0;
            return i;
        }
    }
    return -1;
}

int pipe_write(int id, const char *buffer, uint64_t count){
    if(!valid(id) || buffer==0 || count==0) return -1;
    uint64_t w=0;
    pipe_t *p=&pipes[id];
    while(w < count && p->size < PIPE_BUFFER_CAPACITY){
        p->buf[p->wpos] = buffer[w];
        p->wpos = (p->wpos + 1) % PIPE_BUFFER_CAPACITY;
        p->size++;
        w++;
    }
    return (int)w;
}

int pipe_read(int id, char *buffer, uint64_t count){
    if(!valid(id) || buffer==0 || count==0) return -1;
    uint64_t r=0;
    pipe_t *p=&pipes[id];
    while(r < count && p->size > 0){
        buffer[r] = p->buf[p->rpos];
        p->rpos = (p->rpos + 1) % PIPE_BUFFER_CAPACITY;
        p->size--;
        r++;
    }
    return (int)r;
}


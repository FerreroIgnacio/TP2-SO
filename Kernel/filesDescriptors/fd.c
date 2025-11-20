#include "fd.h"
#include "../scheduler/scheduler.h"
#include "pipes.h"
#include "../utils/lib.h" // para strcpy

// Tabla de FDs por proceso
static fd_entry_t proc_fds[MAX_TASKS][MAX_PROCESS_FDS];
static int bound_stdin_pipe[MAX_TASKS];
static int bound_stdout_pipe[MAX_TASKS];

static inline int valid_pid(int pid) {
    return (pid >= 0 && pid < MAX_TASKS);
}

void fd_init(void) {
    // Crear pipe 0 para stdin global
    int _p0 = pipe_create();
    for (int p = 0; p < MAX_TASKS; p++) {
        for (int i = 0; i < MAX_PROCESS_FDS; i++) {
            proc_fds[p][i].in_use = 0;
            proc_fds[p][i].name[0] = '\0';
            proc_fds[p][i].read_pos = 0;
            proc_fds[p][i].write_pos = 0;
            proc_fds[p][i].size = 0;
        }
        // Initialize standard FDs 0,1,2
        proc_fds[p][0].in_use = 1; strcpy(proc_fds[p][0].name, "stdin");
        proc_fds[p][1].in_use = 1; strcpy(proc_fds[p][1].name, "stdout");
        proc_fds[p][2].in_use = 1; strcpy(proc_fds[p][2].name, "stderr");
        // Enlazar SOLO stdin a pipe 0;
        bound_stdin_pipe[p] = 0;
        bound_stdout_pipe[p] = -1;
    }
}

void fd_reset_pid(int pid) {
    if (!valid_pid(pid)) return;
    for (int i = 0; i < MAX_PROCESS_FDS; i++) {
        proc_fds[pid][i].in_use = 0;
        proc_fds[pid][i].name[0] = '\0';
        proc_fds[pid][i].read_pos = 0;
        proc_fds[pid][i].write_pos = 0;
        proc_fds[pid][i].size = 0;
    }
    proc_fds[pid][0].in_use = 1; strcpy(proc_fds[pid][0].name, "stdin");
    proc_fds[pid][1].in_use = 1; strcpy(proc_fds[pid][1].name, "stdout");
    proc_fds[pid][2].in_use = 1; strcpy(proc_fds[pid][2].name, "stderr");
    // Reenlazar SOLO stdin del nuevo proceso a pipe 0;
    bound_stdin_pipe[pid] = 0;
    bound_stdout_pipe[pid] = -1;
}

static inline fd_entry_t *get_current_table(void) {
    int pid = scheduler_current_pid();
    if (!valid_pid(pid)) return NULL;
    return proc_fds[pid];
}

static inline int idx_from_fd(int fd) {
    int idx = fd - FIRST_DYNAMIC_FD;
    if (idx < 0 || idx >= MAX_PROCESS_FDS) return -1;
    return idx;
}

int fd_bind_std_for_pid(int pid, int which, int pipe_id) {
    if (!valid_pid(pid) || (which != 0 && which != 1)) return -1;
    if (which == 0) bound_stdin_pipe[pid] = pipe_id; else bound_stdout_pipe[pid] = pipe_id;
    return 0;
}

int fd_create(const char *name) {
    fd_entry_t *table = get_current_table();
    if (table == NULL || name == NULL) return -1;
    for (int i = 0; i < MAX_PROCESS_FDS; i++) {
        if (!table[i].in_use) {
            table[i].in_use = 1;
            int j = 0;
            while (j < FD_NAME_MAX - 1 && name[j] != '\0') {
                table[i].name[j] = name[j];
                j++;
            }
            table[i].name[j] = '\0';
            table[i].read_pos = 0;
            table[i].write_pos = 0;
            table[i].size = 0;
            return FIRST_DYNAMIC_FD + i;
        }
    }
    return -1; // sin espacio
}

int fd_write(int fd, const char *buffer, uint64_t count) {
    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (buffer == NULL || count == 0) return -1;

    int pid = scheduler_current_pid();
    if (fd == STDOUT && valid_pid(pid) && bound_stdout_pipe[pid] >= 0) {
        return pipe_write(bound_stdout_pipe[pid], buffer, count); // blocking via pipe
    }
    if (fd == STDIN && valid_pid(pid) && bound_stdin_pipe[pid] >= 0) {
        return pipe_write(bound_stdin_pipe[pid], buffer, count);
    }
    if (table == NULL || idx < 0 || !table[idx].in_use) return -1;
    uint64_t written = 0;
    while (written < count) {
        while (table[idx].size == FD_BUFFER_CAPACITY) {
            scheduler_yield();
        }
        table[idx].buffer[table[idx].write_pos] = (uint8_t)buffer[written];
        table[idx].write_pos = (table[idx].write_pos + 1) % FD_BUFFER_CAPACITY;
        table[idx].size++;
        written++;
    }
    return (int)written;
}

int fd_read(int fd, char *buffer, uint64_t count) {
    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (buffer == NULL || count == 0) return -1;
    int pid = scheduler_current_pid();
    // STDIN: si está redirigido a una pipe usarla; si no, usar buffer propio como cualquier FD
    if (fd == STDIN && valid_pid(pid) && bound_stdin_pipe[pid] >= 0) {
        return pipe_read(bound_stdin_pipe[pid], buffer, count);
    }
    if (fd == STDOUT && valid_pid(pid) && bound_stdout_pipe[pid] >= 0) {
        return pipe_read(bound_stdout_pipe[pid], buffer, count);
    }
    if (table == NULL || idx < 0 || !table[idx].in_use) return -1;
    uint64_t read = 0;
    while (read < count) {
        while (table[idx].size == 0) {
            scheduler_yield();
        }
        buffer[read] = (char)table[idx].buffer[table[idx].read_pos];
        table[idx].read_pos = (table[idx].read_pos + 1) % FD_BUFFER_CAPACITY;
        table[idx].size--;
        read++;
    }
    return (int)read;
}

int fd_has_data(int fd) {
    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (table == NULL || idx < 0 || !table[idx].in_use) return 0;
    return table[idx].size > 0;
}

int fd_list(fd_info_t *out, int max) {
    if (out == NULL || max <= 0) return 0;
    fd_entry_t *table = get_current_table();
    if (table == NULL) return 0;
    int count = 0;
    for (int i = 0; i < MAX_PROCESS_FDS && count < max; i++) {
        if (table[i].in_use) {
            out[count].fd = FIRST_DYNAMIC_FD + i;
            int j = 0;
            while (j < FD_NAME_MAX - 1 && table[i].name[j]) { out[count].name[j] = table[i].name[j]; j++; }
            out[count].name[j] = '\0';
            out[count].size = table[i].size;
            count++;
        }
    }
    return count;
}

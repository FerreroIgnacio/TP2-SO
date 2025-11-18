#include "fd.h"
#include "../scheduler/scheduler.h"
#include <string.h>

// Tabla de FDs por proceso
static fd_entry_t proc_fds[MAX_TASKS][MAX_PROCESS_FDS];
static int bound_stdin_pipe[MAX_TASKS];
static int bound_stdout_pipe[MAX_TASKS];

static inline int valid_pid(int pid) {
    return (pid >= 0 && pid < MAX_TASKS);
}

void fd_init(void) {
    // Limpia todas las tablas
    for (int p = 0; p < MAX_TASKS; p++) {
        for (int i = 0; i < MAX_PROCESS_FDS; i++) {
            proc_fds[p][i].in_use = 0;
            proc_fds[p][i].name[0] = '\0';
            proc_fds[p][i].read_pos = 0;
            proc_fds[p][i].write_pos = 0;
            proc_fds[p][i].size = 0;
        }
        bound_stdin_pipe[p] = -1;
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
    bound_stdin_pipe[pid] = -1;
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

int fd_get_bound_stdin_pipe(void) {
    int pid = scheduler_current_pid();
    if (!valid_pid(pid)) return -1;
    return bound_stdin_pipe[pid];
}

int fd_get_bound_stdout_pipe(void) {
    int pid = scheduler_current_pid();
    if (!valid_pid(pid)) return -1;
    return bound_stdout_pipe[pid];
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
    if (table == NULL || idx < 0 || !table[idx].in_use || buffer == NULL || count == 0) return -1;

    uint64_t written = 0;
    while (written < count && table[idx].size < FD_BUFFER_CAPACITY) {
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
    if (table == NULL || idx < 0 || !table[idx].in_use || buffer == NULL || count == 0) return -1;

    uint64_t read = 0;
    while (read < count && table[idx].size > 0) {
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

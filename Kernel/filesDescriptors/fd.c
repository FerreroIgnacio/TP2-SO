#include "fd.h"

static fd_entry_t fds[MAX_DYNAMIC_FDS];

static int idx_from_fd(int fd) {
    int idx = fd - FIRST_DYNAMIC_FD;
    if (idx < 0 || idx >= MAX_DYNAMIC_FDS) return -1;
    return idx;
}

void fd_init(void) {
    for (int i = 0; i < MAX_DYNAMIC_FDS; i++) {
        fds[i].in_use = 0;
        fds[i].name[0] = '\0';
        fds[i].read_pos = 0;
        fds[i].write_pos = 0;
        fds[i].size = 0;
    }
}

int fd_create(const char *name) {
    if (name == 0) return -1;
    for (int i = 0; i < MAX_DYNAMIC_FDS; i++) {
        if (!fds[i].in_use) {
            fds[i].in_use = 1;
            int j = 0;
            while (j < FD_NAME_MAX - 1 && name[j] != '\0') {
                fds[i].name[j] = name[j];
                j++;
            }
            fds[i].name[j] = '\0';
            fds[i].read_pos = 0;
            fds[i].write_pos = 0;
            fds[i].size = 0;
            return FIRST_DYNAMIC_FD + i;
        }
    }
    return -1; // no space
}

int fd_write(int fd, const char *buffer, uint64_t count) {
    int idx = idx_from_fd(fd);
    if (idx < 0 || !fds[idx].in_use || buffer == 0 || count == 0) return -1;

    uint64_t written = 0;
    while (written < count && fds[idx].size < FD_BUFFER_CAPACITY) {
        fds[idx].buffer[fds[idx].write_pos] = (uint8_t)buffer[written];
        fds[idx].write_pos = (fds[idx].write_pos + 1) % FD_BUFFER_CAPACITY;
        fds[idx].size++;
        written++;
    }
    return (int)written;
}

int fd_read(int fd, char *buffer, uint64_t count) {
    int idx = idx_from_fd(fd);
    if (idx < 0 || !fds[idx].in_use || buffer == 0 || count == 0) return -1;

    uint64_t read = 0;
    while (read < count && fds[idx].size > 0) {
        buffer[read] = (char)fds[idx].buffer[fds[idx].read_pos];
        fds[idx].read_pos = (fds[idx].read_pos + 1) % FD_BUFFER_CAPACITY;
        fds[idx].size--;
        read++;
    }
    return (int)read;
}

int fd_has_data(int fd) {
    int idx = idx_from_fd(fd);
    if (idx < 0 || !fds[idx].in_use) return 0;
    return fds[idx].size > 0;
}

int fd_list(fd_info_t *out, int max) {
    if (out == 0 || max <= 0) return 0;
    int count = 0;
    for (int i = 0; i < MAX_DYNAMIC_FDS && count < max; i++) {
        if (fds[i].in_use) {
            out[count].fd = FIRST_DYNAMIC_FD + i;
            // copy name ensuring null-termination
            int j = 0;
            while (j < FD_NAME_MAX - 1 && fds[i].name[j]) { out[count].name[j] = fds[i].name[j]; j++; }
            out[count].name[j] = '\0';
            out[count].size = fds[i].size;
            count++;
        }
    }
    return count;
}

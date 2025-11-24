#include "fd.h"
#include "../scheduler/scheduler.h"
#include "pipes.h"
#include "../utils/lib.h" // para strcpy

// Tabla de FDs por proceso
static fd_entry_t proc_fds[MAX_TASKS][MAX_PROCESS_FDS];
static int bound_stdin_pipe[MAX_TASKS];
static int bound_stdout_pipe[MAX_TASKS];

static inline int valid_pid(int pid)
{
    return (pid >= 0 && pid < MAX_TASKS);
}

void fd_init(void)
{
    // Crear pipe 0 para stdin global
    pipe_create();
    for (int p = 0; p < MAX_TASKS; p++)
    {
        for (int i = 0; i < MAX_PROCESS_FDS; i++)
        {
            proc_fds[p][i].in_use = 0;
            proc_fds[p][i].name[0] = '\0';
            proc_fds[p][i].read_pos = 0;
            proc_fds[p][i].write_pos = 0;
            proc_fds[p][i].size = 0;
            spinlock_init(&proc_fds[p][i].lock);
        }
        // Initialize standard FDs 0,1,2
        proc_fds[p][0].in_use = 1;
        strcpy(proc_fds[p][0].name, "stdin");
        proc_fds[p][1].in_use = 1;
        strcpy(proc_fds[p][1].name, "stdout");
        proc_fds[p][2].in_use = 1;
        strcpy(proc_fds[p][2].name, "stderr");
        // Enlazar SOLO stdin a pipe 0
        bound_stdin_pipe[p] = 0;
        bound_stdout_pipe[p] = -1;
    }
}

void fd_reset_pid(int pid)
{
    if (!valid_pid(pid))
        return;
    for (int i = 0; i < MAX_PROCESS_FDS; i++)
    {
        proc_fds[pid][i].in_use = 0;
        proc_fds[pid][i].name[0] = '\0';
        proc_fds[pid][i].read_pos = 0;
        proc_fds[pid][i].write_pos = 0;
        proc_fds[pid][i].size = 0;
        spinlock_init(&proc_fds[pid][i].lock);
    }
    proc_fds[pid][0].in_use = 1;
    strcpy(proc_fds[pid][0].name, "stdin");
    proc_fds[pid][1].in_use = 1;
    strcpy(proc_fds[pid][1].name, "stdout");
    proc_fds[pid][2].in_use = 1;
    strcpy(proc_fds[pid][2].name, "stderr");
    // Reenlazar SOLO stdin del nuevo proceso a pipe 0
    bound_stdin_pipe[pid] = 0;
    bound_stdout_pipe[pid] = -1;
}

static inline fd_entry_t *get_current_table(void)
{
    int pid = scheduler_current_pid();
    if (!valid_pid(pid))
        return NULL;
    return proc_fds[pid];
}

static inline int idx_from_fd(int fd)
{
    int idx = fd - FIRST_DYNAMIC_FD;
    if (idx < 0 || idx >= MAX_PROCESS_FDS)
        return -1;
    return idx;
}

int fd_bind_std_for_pid(int pid, int whichPipe, int pipeId)
{
    if (!valid_pid(pid) || (whichPipe != 0 && whichPipe != 1))
        return -1;
    if (whichPipe == 0)
        bound_stdin_pipe[pid] = pipeId;
    else
        bound_stdout_pipe[pid] = pipeId;
    return 0;
}

int fd_create(const char *name)
{
    fd_entry_t *table = get_current_table();
    if (table == NULL || name == NULL)
        return -1;
    for (int i = 0; i < MAX_PROCESS_FDS; i++)
    {
        // Lock para verificar y marcar como en uso atómicamente
        spinlock_lock(&table[i].lock);
        if (!table[i].in_use)
        {
            table[i].in_use = 1;
            int j = 0;
            while (j < FD_NAME_MAX - 1 && name[j] != '\0')
            {
                table[i].name[j] = name[j];
                j++;
            }
            table[i].name[j] = '\0';
            table[i].read_pos = 0;
            table[i].write_pos = 0;
            table[i].size = 0;
            spinlock_unlock(&table[i].lock);
            return FIRST_DYNAMIC_FD + i;
        }
        spinlock_unlock(&table[i].lock);
    }
    return -1;
}

int fd_write(int fd, const char *buffer, uint64_t count)
{
    if (buffer == NULL || count == 0)
        return -1;

    // CRÍTICO: Validar pid ANTES de usar arrays globales
    int pid = scheduler_current_pid();
    if (!valid_pid(pid))
        return -1;

    // Si está redirigido a pipe, usar semántica parcial de pipe_write
    if (fd == STDOUT && bound_stdout_pipe[pid] >= 0)
    {
        return pipe_write(bound_stdout_pipe[pid], buffer, count);
    }
    if (fd == STDIN && bound_stdin_pipe[pid] >= 0)
    {
        return pipe_write(bound_stdin_pipe[pid], buffer, count);
    }

    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (table == NULL || idx < 0)
        return -1;

    // Verificar que el FD esté en uso
    spinlock_lock(&table[idx].lock);
    if (!table[idx].in_use)
    {
        spinlock_unlock(&table[idx].lock);
        return -1;
    }
    spinlock_unlock(&table[idx].lock);

    uint64_t written = 0;
    while (written < count)
    {
        spinlock_lock(&table[idx].lock);

        if (table[idx].size == FD_BUFFER_CAPACITY)
        {
            // Buffer lleno. Si ya escribimos algo, devolver parcial.
            if (written > 0)
            {
                spinlock_unlock(&table[idx].lock);
                break;
            }
            // Nada escrito: liberar lock y yield hasta que haya espacio
            spinlock_unlock(&table[idx].lock);
            scheduler_yield();
            continue; // Reintentar desde el principio del loop
        }

        // Hay al menos 1 byte de espacio: escribir chunk
        uint64_t remaining = count - written;
        uint32_t free_space = FD_BUFFER_CAPACITY - table[idx].size;
        uint64_t chunk = (remaining < free_space) ? remaining : free_space;

        // Escribir con el lock tomado para evitar race conditions
        for (uint64_t i = 0; i < chunk; i++)
        {
            table[idx].buffer[table[idx].write_pos] = (uint8_t)buffer[written + i];
            table[idx].write_pos = (table[idx].write_pos + 1) % FD_BUFFER_CAPACITY;
        }
        table[idx].size += (uint32_t)chunk;
        written += chunk;

        spinlock_unlock(&table[idx].lock);

        // Si se llenó y quedan datos, salimos con parcial
        if (written < count)
        {
            spinlock_lock(&table[idx].lock);
            int is_full = (table[idx].size == FD_BUFFER_CAPACITY);
            spinlock_unlock(&table[idx].lock);
            if (is_full)
                break;
        }
    }
    return (int)written;
}

int fd_read(int fd, char *buffer, uint64_t count)
{
    if (buffer == NULL || count == 0)
        return -1;

    // CRÍTICO: Validar pid ANTES de usar arrays globales
    int pid = scheduler_current_pid();
    if (!valid_pid(pid))
        return -1;

    // Pipes ya manejan parcial
    if (fd == STDIN && bound_stdin_pipe[pid] >= 0)
    {
        return pipe_read(bound_stdin_pipe[pid], buffer, count);
    }
    if (fd == STDOUT && bound_stdout_pipe[pid] >= 0)
    {
        return pipe_read(bound_stdout_pipe[pid], buffer, count);
    }

    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (table == NULL || idx < 0)
        return -1;

    // Verificar que el FD esté en uso
    spinlock_lock(&table[idx].lock);
    if (!table[idx].in_use)
    {
        spinlock_unlock(&table[idx].lock);
        return -1;
    }
    spinlock_unlock(&table[idx].lock);

    uint64_t read = 0;
    while (read < count)
    {
        spinlock_lock(&table[idx].lock);

        if (table[idx].size == 0)
        {
            // Si ya leímos algo, devolver parcial
            if (read > 0)
            {
                spinlock_unlock(&table[idx].lock);
                break;
            }
            // Bloquear/yield hasta que haya al menos 1 byte
            spinlock_unlock(&table[idx].lock);
            scheduler_yield();
            continue; // Reintentar
        }

        // Hay datos: leer chunk
        uint64_t remaining = count - read;
        uint32_t available = table[idx].size;
        uint64_t chunk = (remaining < available) ? remaining : available;

        // Leer con el lock tomado
        for (uint64_t i = 0; i < chunk; i++)
        {
            buffer[read + i] = (char)table[idx].buffer[table[idx].read_pos];
            table[idx].read_pos = (table[idx].read_pos + 1) % FD_BUFFER_CAPACITY;
        }
        table[idx].size -= (uint32_t)chunk;
        read += chunk;

        spinlock_unlock(&table[idx].lock);

        // Si se vació y quedan datos por leer, salir con parcial
        if (read < count)
        {
            spinlock_lock(&table[idx].lock);
            int is_empty = (table[idx].size == 0);
            spinlock_unlock(&table[idx].lock);
            if (is_empty)
                break;
        }
    }
    return (int)read;
}

int fd_has_data(int fd)
{
    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (table == NULL || idx < 0)
        return 0;

    spinlock_lock(&table[idx].lock);
    if (!table[idx].in_use)
    {
        spinlock_unlock(&table[idx].lock);
        return 0;
    }
    int has_data = table[idx].size > 0;
    spinlock_unlock(&table[idx].lock);

    return has_data;
}

int fd_list(fd_info_t *out, int max)
{
    if (out == NULL || max <= 0)
        return 0;
    fd_entry_t *table = get_current_table();
    if (table == NULL)
        return 0;
    int count = 0;
    for (int i = 0; i < MAX_PROCESS_FDS && count < max; i++)
    {
        spinlock_lock(&table[i].lock);
        if (table[i].in_use)
        {
            out[count].fd = FIRST_DYNAMIC_FD + i;
            int j = 0;
            while (j < FD_NAME_MAX - 1 && table[i].name[j])
            {
                out[count].name[j] = table[i].name[j];
                j++;
            }
            out[count].name[j] = '\0';
            out[count].size = table[i].size;
            count++;
        }
        spinlock_unlock(&table[i].lock);
    }
    return count;
}

int fd_get_bound_std_pipe(int pid, int whichPipe)
{
    if (!valid_pid(pid) || (whichPipe != 0 && whichPipe != 1))
        return -1;
    return (whichPipe == 0) ? bound_stdin_pipe[pid] : bound_stdout_pipe[pid];
}

int fd_is_read_ready(int fd)
{
    int pid = scheduler_current_pid();
    if (!valid_pid(pid))
        return -1;

    if (fd == STDIN)
    {
        int pipeId = fd_get_bound_std_pipe(pid, 0);
        if (pipeId >= 0)
            return pipe_available(pipeId) > 0 ? 1 : 0;
    }
    else if (fd == STDOUT)
    {
        int pipeId = fd_get_bound_std_pipe(pid, 1);
        if (pipeId >= 0)
            return pipe_available(pipeId) > 0 ? 1 : 0;
    }

    fd_entry_t *table = get_current_table();
    int idx = idx_from_fd(fd);
    if (table == NULL || idx < 0)
        return -1;

    spinlock_lock(&table[idx].lock);
    if (!table[idx].in_use)
    {
        spinlock_unlock(&table[idx].lock);
        return -1;
    }
    int ready = table[idx].size > 0 ? 1 : 0;
    spinlock_unlock(&table[idx].lock);

    return ready;
}
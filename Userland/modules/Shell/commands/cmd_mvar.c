#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/semaphores/semaphores.h"
#include "../../../libs/process/process.h"
#include "../../../libs/time/time.h"
#include "../../../libs/mystring/mystring.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"

static void mvar_proc(void *argv)
{
    if (argv == NULL)
        return;
    char **args = (char **)argv;
    char *shared_mm = args[0];
    char *mutex_sem_name = args[1];
    char *print_sem_name = args[2];
    if (!shared_mm || !mutex_sem_name || !print_sem_name)
        return;

    int mutex_sem_id = sem_open(mutex_sem_name, 1);
    int print_sem_id = sem_open(print_sem_name, 0);
    if (mutex_sem_id == -1 || print_sem_id == -1)
        return;

    while (1)
    {
        sem_wait(mutex_sem_id);
        printf("Proceso %d accediendo a la memoria compartida\n", (int)getpid());
        sleep(1);
        printf("Proceso %d leyendo memoria: %s\n", (int)getpid(), shared_mm);
        sleep(1);
        printf("Proceso %d escribiendo memoria\n", (int)getpid());
        strcpy(shared_mm, "El proceso: ");
        char *pid_str = itoa_malloc((int)getpid());
        if (pid_str)
        {
            strcpy(shared_mm + strlen(shared_mm), pid_str);
            free(pid_str);
        }
        else
        {
            strcpy(shared_mm + strlen(shared_mm), "unknown PID");
        }
        strcpy(shared_mm + strlen(shared_mm), " estuvo aqui.\n");
        sem_post(print_sem_id);
        sem_post(mutex_sem_id);
    }
}

static int mvar_main(void *unused)
{
    char *shared_mm = (char *)calloc(1000, sizeof(char));
    char *mutex_name = "mvar_mutex";
    char *print_name = "mvar_print";
    int print_sem_id;
    int pipes[3];
    pid_t pids[3];
    if (shared_mm == NULL)
    {
        printf("Error memoria compartida\n");
        return -1;
    }
    if ((print_sem_id = sem_open(print_name, 0)) == -1)
    {
        printf("Error sem print\n");
        return -1;
    }
    char *argv[] = {shared_mm, mutex_name, print_name};
    for (int i = 0; i < 3; i++)
    {
        pipes[i] = pipe_create();
        if (pipes[i] == -1)
        {
            printf("Error pipe %d\n", i);
            return -1;
        }
        pids[i] = new_proc((task_fn_t)mvar_proc, argv);
        fd_bind_std(pids[i], STDOUT, pipes[i]);
    }
    int next = 0;
    while (1)
    {
        sem_wait(print_sem_id);
        unsigned char buffer[128];
        for (int i = 0; i < 3; i++)
        {
            if (!fd_has_data(pipes[next]))
            {
                next = (next + 1) % 3;
                continue;
            }
            int n = read(pipes[next], buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = '\0';
                printf("%s", buffer);
            }
            next = (next + 1) % 3;
            break;
        }
    }
    return 0;
}

int cmd_mvar_run(int argc, char **argv)
{
    run_in_foreground((task_fn_t)mvar_main, NULL);
    return 0;
}

#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/process/process.h"

int cmd_ps_run(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    proc_info_t infos[100];
    int n = get_proc_list(infos, 100);
    for (int i = 0; i < n; i++)
    {
        proc_info_t *p = &infos[i];
        printf("PID:%d Father:%d Pri:%d Ready:%d Wait:%d Zombie:%d Status:%d\n", p->pid, p->father_pid, p->priority, p->ready, p->waiting, p->is_zombie, p->status);
    }
    exit(0);
    return 0;
}

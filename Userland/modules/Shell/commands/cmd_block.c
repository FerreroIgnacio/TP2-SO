#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/process/process.h"

int cmd_block_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    pid_t pid = strtoint(argv[0]);
    if (block_proc(pid) == 0)
    {
        printf("bloqueando el proceso: %d\n", pid);
    }
    else
    {
        printf("desbloqueando el proceso: %d\n", pid);
        unblock_proc(pid);
    }
    return 0;
}

#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/process/process.h"

int cmd_kill_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    pid_t pid = strtoint(argv[0]);
    int status = kill(pid);
    printf("Kill a proceso: %d termino con estado: %d \n", pid, status);
    exit(0);
    return 0;
}

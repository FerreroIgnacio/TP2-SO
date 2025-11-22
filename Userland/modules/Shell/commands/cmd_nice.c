#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/process/process.h"

int cmd_nice_run(int argc, char **argv)
{
    if (argc < 2)
        return -1;
    pid_t pid = strtoint(argv[0]);
    process_priority_t prio = strtoint(argv[1]);
    if (prio < PRIORITY_LOW || prio > PRIORITY_HIGH)
    {
        printf("Error, prioridades disponibles:\nLOW : %d\nNORMAL : %d\nHIGH: %d\n", PRIORITY_LOW, PRIORITY_NORMAL, PRIORITY_HIGH);
        return -1;
    }
    printf("cambiando la prioridad del proceso: %d a %d\n", pid, prio);
    set_priority(pid, prio);
    exit(0);
    return 0;
}

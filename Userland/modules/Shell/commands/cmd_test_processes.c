#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/test/test.h"
#include "../../../libs/standard/standard.h"

static int processes_proc(void *arg)
{
    int max = *(int *)arg;
    printf("Iniciando testProcesses con max %d procesos...\n", max);
    int result = test_processes(max);
    printf("testProcesses finalizado con codigo %d\n", result);
    return result;
}

int cmd_test_processes_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int max = strtoint(argv[0]);
    if (max <= 1)
    {
        printf("Uso: test_processes <max-processes>\n");
        return -1;
    }
    run_in_foreground(processes_proc, &max);
    exit(0);
    return 0;
}

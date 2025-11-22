#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/test/test.h"
#include "../../../libs/standard/standard.h"

static int priority_proc(void *arg)
{
    int end = *(int *)arg;
    printf("Iniciando testPriority con max_val %d ...\n", end);
    int result = test_prio(end);
    printf("testPriority finalizado con codigo %d\n", result);
    return result;
}

int cmd_test_priority_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int end = strtoint(argv[0]);
    if (end <= 1)
    {
        printf("Uso: test_priority <end-val>\n");
        return -1;
    }
    run_in_foreground(priority_proc, &end);
    return 0;
}

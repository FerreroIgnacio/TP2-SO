#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/test/test.h"
#include "../../../libs/standard/standard.h"

static int no_synchro_proc(void *arg)
{
    int *p = (int *)arg;
    int val = p[0];
    printf("Iniciando test_no_synchro val=%d ...\n", val);
    int result = test_sync(val, 0);
    printf("test_no_synchro finalizado con codigo %d\n", result);
    exit(result);
    return result;
}

int cmd_test_no_synchro_run(int argc, char **argv)
{
    if (argc < 2)
        return -1;
    int params[2] = {strtoint(argv[0]), 0};
    run_in_foreground(no_synchro_proc, params);
    return 0;
}

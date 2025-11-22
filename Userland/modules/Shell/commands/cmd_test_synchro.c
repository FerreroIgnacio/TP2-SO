#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/test/test.h"
#include "../../../libs/standard/standard.h"

static int synchro_proc(void *arg)
{
    int *p = (int *)arg;
    int val = p[0];
    int mode = p[1];
    printf("Iniciando test_synchro val=%d mode=%d ...\n", val, mode);
    int result = test_sync(val, mode);
    printf("test_synchro finalizado con codigo %d\n", result);
    return result;
}

int cmd_test_synchro_run(int argc, char **argv)
{
    if (argc < 2)
        return -1;
    int params[2] = {strtoint(argv[0]), strtoint(argv[1])};
    run_in_foreground(synchro_proc, params);
    return 0;
}

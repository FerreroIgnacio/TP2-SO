#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/test/test.h"
#include "../../../libs/standard/standard.h"

static int mm_proc(void *arg)
{
    int max = *(int *)arg;
    printf("Iniciando testMM con %d bytes\n", max);
    int result = test_mm(max);
    printf("testMM finalizado con codigo %d\n", result);
    return result;
}

int cmd_test_mm_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int max = strtoint(argv[0]);
    if (max <= 1)
    {
        printf("Uso: test_mm <max-bytes>\n");
        return -1;
    }
    run_in_foreground(mm_proc, &max);
    exit(0);
    return 0;
}

#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/time/time.h"
#include "../../../libs/process/process.h"

static int loop_proc(void *arg)
{
    int segs = *(int *)arg;
    while (1)
    {
        printf("Hola! soy el proceso: %d. Este mensaje aparecera cada %d segundos \n", (int)getpid(), segs);
        sleep(segs);
    }
    return 0;
}

int cmd_loop_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int segs = strtoint(argv[0]);
    if (segs <= 0)
    {
        printf("Uso: loop <segundos>\n");
        return -1;
    }
    int *a = malloc(sizeof(int));
    if (!a)
        return -1;
    *a = segs;
    run_in_foreground(loop_proc, a);
    return 0;
}

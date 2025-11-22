#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"

static int wc_proc(void *unused)
{
    int count = 0;
    while (1)
    {
        unsigned char c = getchar();
        if (c == '\0')
            break;
        if (c == '\n')
            count++;
    }
    printf("Lineas: %d\n", count);
    return count;
}

int cmd_wc_run(int argc, char **argv)
{
    run_in_foreground((task_fn_t)wc_proc, NULL);
    return 0;
}

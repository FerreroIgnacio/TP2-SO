#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/process/process.h"

static int cat_proc(void *unused)
{
    while (1)
    {
        unsigned char c = getchar();
        if (c == 'A')
        {
            printf("\n");
            break;
        }
        putchar(c);
    }
    return 0;
}

int cmd_cat_run(int argc, char **argv)
{
    run_in_foreground((task_fn_t)cat_proc, NULL);
    exit(0);
    return 0;
}

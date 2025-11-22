#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/mystring/mystring.h"

static int filter_proc(void *unused)
{
    while (1)
    {
        unsigned char c = getchar();
        if (c == '\0')
            break;
        if (strchr("aeiouAEIOU", c))
            putchar(c);
    }
    return 0;
}

int cmd_filter_run(int argc, char **argv)
{
    run_in_foreground((task_fn_t)filter_proc, NULL);
    return 0;
}

#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/mystring/mystring.h"

int cmd_filter_run(int argc, char **argv)
{
    while (1)
    {
        unsigned char c = getchar();
        if (c == '\0')
            break;
        if (strchr("aeiouAEIOU", c))
            putchar(c);
    }
    exit(0);
    return 0;
}

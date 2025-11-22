#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/mystring/mystring.h"
#include "../shell_render.h"

int echo_proc(void *argv)
{
    char *arg = (char *)argv;
    if (arg && *arg)
        write(STDOUT, arg, strlen(arg));
    write(STDOUT, "\n", 1);
    if (arg)
        free(arg);
    return 0;
}

void cmd_echo(char *args)
{
    if (args && *args)
        printf("%s\n", args);
    else
        shell_newline();
}

int cmd_echo_run(int argc, char **argv)
{
    cmd_echo(argc > 0 ? argv[0] : "");
    return 0;
}

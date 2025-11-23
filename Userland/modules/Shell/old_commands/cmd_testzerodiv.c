#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"

int cmd_testzerodiv_run(int argc, char **argv)
{
    int x = 1;
    int y = 0;
    int z = x / y;
    (void)z;
    return 0;
}

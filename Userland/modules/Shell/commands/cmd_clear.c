#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"

int cmd_clear_run(int argc, char **argv)
{
    clear_screen();
    return 0;
}

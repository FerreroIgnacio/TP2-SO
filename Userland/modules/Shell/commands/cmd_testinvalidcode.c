#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/invalidOpCode/invalidOpCode.h"

int cmd_testinvalidcode_run(int argc, char **argv)
{
    testInvalidCode();
    return 0;
}

#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/memory/memory.h"
#include "../../../libs/standard/standard.h"

int cmd_meminfo_run(int argc, char **argv)
{
    size_t total, used, freeMem;
    getMemInfo(&total, &used, &freeMem);
    printf("Mem TOTAL:%u USADO:%u LIBRE:%u\n", (unsigned)total, (unsigned)used, (unsigned)freeMem);
    return 0;
}

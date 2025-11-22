#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/memory/memory.h"

int cmd_mem_run(int argc, char **argv)
{
    size_t total = 0, used = 0, freeMem = 0;
    getMemInfo(&total, &used, &freeMem);
    printf("Estado de la memoria:\n");
    printf("TOTAL: %llu   USADO: %llu   LIBRE: %llu\n", (unsigned long long)total, (unsigned long long)used, (unsigned long long)freeMem);
    exit(0);
    return 0;
}

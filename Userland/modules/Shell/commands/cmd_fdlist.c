#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"

int cmd_fdlist_run(int argc, char **argv)
{
    fd_info_u_t infos[32];
    int n = fd_list(infos, 32);
    if (n <= 0)
    {
        printf("(sin FDs dinamicos en este proceso)\n");
        return 0;
    }
    printf("FDs dinamicos del proceso actual (%d):\n", n);
    for (int i = 0; i < n; i++)
        printf("  id=%d name=%s bytes=%u\n", infos[i].fd, infos[i].name, (unsigned)infos[i].size);
    return 0;
}

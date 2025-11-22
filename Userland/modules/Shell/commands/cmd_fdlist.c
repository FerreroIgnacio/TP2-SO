#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"
#include "../shell_render.h"
int cmd_fdlist_run(int argc, char **argv)
{
    fd_info_u_t infos[32];
    int n = fd_list(infos, 32);
    if (n <= 0)
    {
        fprintf(3, "(sin FDs dinamicos en este proceso)\n");
        return 0;
    }
    fprintf(3, "FDs dinamicos del proceso actual (%d):\n", n);
    for (int i = 0; i < n; i++)
        fprintf(3,"  id=%d name=%s bytes=%u\n", infos[i].fd, infos[i].name, (unsigned)infos[i].size);
   // exit(0);
    consume_render_fd();
    return 0;
}

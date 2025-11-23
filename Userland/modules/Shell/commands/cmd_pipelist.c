#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fileDescriptorUtils/fileDescriptorUtils.h"
#include "../../../libs/standard/standard.h"
#include "../shell_render.h"

// pipelist: muestra estados de pipes de kernel
int cmd_pipelist_run(int argc, char **argv)
{
    pipe_info_u_t infos[64];
    int n = pipe_list_u(infos, 64);
    if (n <= 0){
        fprintf(3, "(no hay pipes o error)\n");
        return 0;
    }
    fprintf(3, "Pipes de kernel (%d):\n", n);
    //DEBUG HARDCODEADO LAS PRIMERAS 6
    for (int i = 0; i < 6; i++){
        fprintf(3, "  id=%d in_use=%d size=%u/%u Rwait=%u Wwait=%u\n",
                infos[i].id,
                infos[i].in_use,
                infos[i].size,
                infos[i].capacity,
                infos[i].readers_waiting,
                infos[i].writers_waiting);
    }
    consume_render_fd();
    return 0;
}

#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/fontManager/fontManager.h"
#include "../shell_render.h"

static void *const pongisgolfModuleAddress = (void *)0x11000000;

int cmd_pong_run(int argc, char **argv)
{
    int current_font = fontmanager_get_current_font_index();
    EntryPoint run = (EntryPoint)pongisgolfModuleAddress;
    if (run != NULL)
        run();
    fontmanager_set_font(current_font);
    clear_screen();
    return 0;
}

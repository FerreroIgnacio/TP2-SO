#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/fontManager/fontManager.h"

void shell_list_fonts()
{
    int count = fontmanager_get_font_count();
    printf("Fuentes disponibles:\n");
    for (int i = 0; i < count; i++)
    {
        printf("  Id: %d - %s\n", i, fontmanager_get_font_name(i));
    }
}

int cmd_listfonts_run(int argc, char **argv)
{
    shell_list_fonts();
    return 0;
}

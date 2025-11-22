#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/fontManager/fontManager.h"
#include "../shell_render.h"

void shell_set_font(font_type_t font_index)
{
    int count = fontmanager_get_font_count();
    if (font_index < 0 || font_index >= count)
    {
        shell_print_colored("Error: indice de fuente inválido\n", ERROR_COLOR);
        return;
    }
    clear_screen();
    fontmanager_set_font(font_index);
    shell_print_colored("Fuente cambiada a: ", PROMPT_COLOR);
    printf("%s\n", fontmanager_get_font_name(font_index));
    shell_newline();
}

int cmd_setfont_run(int argc, char **argv)
{
    if (argc < 1)
        return -1;
    int idx = strtoint(argv[0]);
    shell_set_font(idx);
    return 0;
}

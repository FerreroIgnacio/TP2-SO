#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"

int cmd_help_run()
{
    for (int i = 0; i < shell_commands_count; i++)
    {
        shell_cmd_t *c = &shell_commands[i];
        printf("  %-15s - %s\n", c->name, c->help ? c->help : "(sin descripcion)");
    }
    printf("\nUsa: <comando> [args] | Mira la tabla para usage.\n");
    return 0;
}

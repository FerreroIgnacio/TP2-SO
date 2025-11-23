#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../shell_render.h"

// Helper local para convertir un entero a string decimal usando un buffer estatico.
// Nota: el puntero retornado se invalida con la siguiente llamada (no reentrante).
int cmd_help_run(void *argv)
{
    for (int i = 0; i < shell_commands_count; i++)
    {
        shell_cmd_t *c = &shell_commands[i];
        printf("  %s - %s\n", c->name, c->help ? c->help : "(sin descripcion)");
    }

    // printf("\nUsa: <comando> [args] | Mira la tabla para usage.\n");
    exit(0);
    return 0;
}
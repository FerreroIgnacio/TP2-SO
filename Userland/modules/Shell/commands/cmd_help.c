#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../shell_render.h"

// Helper local para convertir un entero a string decimal usando un buffer estatico.
// Nota: el puntero retornado se invalida con la siguiente llamada (no reentrante).
int cmd_help_run()
{
   // sleep(2);
   // shell_print_colored("Se lanzo help", 0xFF00FF);
    int n = 0;
    for (int i = 0; i < shell_commands_count; i++)
    {
        shell_cmd_t *c = &shell_commands[i];
         n += fprintf(1, "  %s - %s\n", c->name, c->help ? c->help : "(sin descripcion)");
    }

     n += fprintf(1, "\nUsa: <comando> [args] | Mira la tabla para usage.\n");
    exit(0);
    return 0;
}
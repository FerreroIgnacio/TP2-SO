#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

// ejecuta el comando cmd_copy con argumetos args
void command_switch(char *cmd_copy, char *args);

// lanza un programa externo por nombre; retorna pid o -1
int shell_launch_program(const char *name, char *args);

// builtin: echo argumentos a STDOUT
void cmd_echo(char *args);

#endif
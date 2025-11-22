#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

// ejecuta el comando cmd_copy con argumetos args
void command_switch(char *cmd_copy, char *args);

// builtin: echo argumentos a STDOUT
void cmd_echo(char *args);

#endif
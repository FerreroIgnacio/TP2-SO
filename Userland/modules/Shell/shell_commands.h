#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include "../../libs/process/process.h"

// ejecuta el comando cmd_copy con argumetos args
void command_switch(char *cmd_copy, char *args);

pid_t get_foreground_proc();
void set_foreground_proc(pid_t pid);

// builtin: echo argumentos a STDOUT
void cmd_echo(char *args);

#endif
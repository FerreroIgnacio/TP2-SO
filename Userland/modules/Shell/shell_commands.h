#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include "../../libs/process/process.h"

void execute_tokenized_command(char **tokens, int token_count, int foreground_mode, int left_pipe_args, int right_pipe_args);

pid_t get_left_fg_proc();
pid_t get_right_fg_proc();
void set_left_fg_proc(pid_t pid);
void set_right_fg_proc(pid_t pid);
#endif
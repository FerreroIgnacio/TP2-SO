#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include "../../libs/process/process.h"

void execute_tokenized_command(char **tokens, int token_count, int foreground_mode, int left_pipe_args, int right_pipe_args);

#endif
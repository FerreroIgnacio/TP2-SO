#ifndef SHELLDEFS_H
#define SHELLDEFS_H

#include "../../libs/fileDescriptorUtils/fileDescriptorUtils.h"

#define FONT_BMP_SIZE 8
#define FONT_SIZE 1
#define BUFFER_SIZE 256
#define PROMPT_LEN 1

//Todo lo que se escriba mas alla de 4096 en una linea se ignora
#define LINE_BUFFER_SIZE FD_SIZE

#define LINE_Y_PADDING 4
#define LINES_PER_SCREEN height / ((FONT_SIZE * FONT_BMP_SIZE) + LINE_Y_PADDING)
#define STDOUT_BUFFER_SIZE 4096

#define SHELL_COLOR 0x00000A
#define FONT_COLOR 0xAAAAAA
#define ERROR_COLOR 0xAA4444
#define PROMPT_COLOR 0x44AAA4
#define CURSOR_BLINK_INTERVAL 150 // en ticks


#define LSHIFT_MAKECODE 0x2A
#define RSHIFT_MAKECODE 0x36
#define LCTRL_MAKECODE 0x1D
#define LALT_MAKECODE 0x38
#define TAB_MAKECODE 0x0F
#define ESC_MAKECODE 0x01

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define RENDER_FD 3
#define COMMAND_FD 4
typedef int (*EntryPoint)();

#endif
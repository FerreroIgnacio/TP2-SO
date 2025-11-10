#ifndef SHELLRENDER_H
#define SHELLRENDER_H
#include <stdint.h>

void shell_putchar(unsigned char c);
void shell_print(const char *str);
void shell_print_colored(const char *str, uint32_t color);
void shell_newline();
void shell_print_prompt();
void clear_buffer();
void clear_screen();
void hide_cursor();
void reset_cursor();
void update_cursor();

#endif
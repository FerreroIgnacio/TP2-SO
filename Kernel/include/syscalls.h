#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2



#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_PUT_PIXEL 10
#define SYSCALL_ISKEYDOWN 2
int sys_write(int fd, const char* buffer, uint64_t count);

int sys_read(int fd, char* buffer, uint64_t count);

void sys_put_pixel(uint32_t hexColor, uint64_t x, uint64_t y);

int sys_isKeyDown(int scancode);

#endif

#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>

#define STDIN_ADDR 0xFFFFAA
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1

int sys_write(int fd, const char* buffer, uint64_t count);

int sys_read(int fd, char* buffer, uint64_t count);

#endif

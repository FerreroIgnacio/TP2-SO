#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2



#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_ISKEYDOWN 2
#define SYSCALL_GETTIME 3

#define SYSCALL_GET_VIDEO_DATA 9
#define SYSCALL_PUT_PIXEL 10
#define SYSCALL_SET_FRAMEBUFFER 11

int sys_write(int fd, const char* buffer, uint64_t count);

int sys_read(int fd, char* buffer, uint64_t count);

int sys_isKeyDown(int scancode);

void sys_get_video_data(uint16_t* width, uint16_t* height, uint16_t* bpp, uint16_t* pitch);

void sys_put_pixel(uint32_t hexColor, uint64_t x, uint64_t y);

void sys_set_framebuffer(uint8_t * fb);

uint64_t sys_getTime();

#endif

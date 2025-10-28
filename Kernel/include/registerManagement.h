//
// Created by nacho on 10/28/2025.
//

#ifndef TP2_SO_SYS_SETREGISTERS_H
#define TP2_SO_SYS_SETREGISTERS_H

#include <stdint.h>

typedef struct reg_screenshot{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t rip;
} reg_screenshot_t;

void kernel_setRegisters(reg_screenshot_t* regs);

void kernel_getRegisters(reg_screenshot_t* regs);

#endif //TP2_SO_SYS_SETREGISTERS_H

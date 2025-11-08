//
// Created by nacho on 10/28/2025.
//

#ifndef TP2_SO_SYS_SETREGISTERS_H
#define TP2_SO_SYS_SETREGISTERS_H

#include <stdint.h>

typedef struct reg_screenshot
{
    uint64_t rax;    // 0
    uint64_t rbx;    // 1
    uint64_t rcx;    // 2
    uint64_t rdx;    // 3
    uint64_t rsi;    // 4
    uint64_t rdi;    // 5
    uint64_t rbp;    // 6
    uint64_t rsp;    // 7
    uint64_t r8;     // 8
    uint64_t r9;     // 9
    uint64_t r10;    // 10
    uint64_t r11;    // 11
    uint64_t r12;    // 12
    uint64_t r13;    // 13
    uint64_t r14;    // 14
    uint64_t r15;    // 15
    uint64_t rflags; // 16
    uint64_t rip;    // 17
    uint64_t CS;     // 18
    uint64_t SS;     // 19
} reg_screenshot_t;

// return from interrupt with given registers
void interrupt_setRegisters(reg_screenshot_t *regs);

#endif // TP2_SO_SYS_SETREGISTERS_H

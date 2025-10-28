//
// Created by nacho on 10/28/2025.
//
#include <registerManagement.h>
#include <syscalls.h>

void kernel_getRegisters(reg_screenshot_t* regs){
    sys_getRegisters((uint64_t*) regs);
}
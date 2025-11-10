#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "../fontManager/fontManager.h"

/* CAPTURA DE REGISTROS */
// Se define struct registers para mejorar la sintaxis.
typedef struct registers
{
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
} registers_t;
// Guarda en regs la captura de los registros realizada con F1
void getRegisters(registers_t *regs);
// Alias solicitado: readregister() lee la captura de registros en el mismo formato
void readregister(registers_t *regs);

uint32_t rand();

#endif

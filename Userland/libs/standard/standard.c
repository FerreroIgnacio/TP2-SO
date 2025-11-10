#include "standard.h"
#include <stdarg.h>
#include "../mystring/mystring.h"

static uint64_t rand_seed = 0;
void autoinit()
{
    if (rand_seed == 0)
    {
        rand_seed = getBootTime();
    }
}

uint32_t rand()
{
    autoinit();
    // LCG formula
    rand_seed = rand_seed * 1664525ULL + 1013904223ULL;
    return (uint32_t)(rand_seed >> 32);
}

/* CAPTURA DE REGISTROS */
void readregister(registers_t *regs)
{
    if (regs == 0)
        return;
    getRegisters(regs);
}

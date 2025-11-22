#include "commands.h"
#include "../shell_commands.h"
#include "../../../libs/standard/standard.h"
#include "../../../libs/process/process.h"

int cmd_registers_run(int argc, char **argv)
{
    registers_t regs;
    getRegisters(&regs);
    printf("RIP: %#P\nRSP: %#P\nRAX: %#P\n", regs.rip, regs.rsp, regs.rax);
    return 0;
}

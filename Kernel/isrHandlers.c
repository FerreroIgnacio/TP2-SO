#include <isrHandlers.h>
#include <stdout.h>
#include <stdint.h>
#include <lib.h>


typedef struct registers {
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t r8;
    uint64_t r9;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rflags;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
} registers_t;

static void loadRegsToStdout(registers_t * reg){
    char buffer [19];
    queueStringStdout("RIP: ");
    uint64ToHex(reg->rip, buffer);
    queueStringStdout(buffer);

    queueStringStdout("   RSP: ");
    uint64ToHex(reg->rip, buffer);
    queueStringStdout(buffer);
    queueStringStdout("\n");
    
    queueStringStdout("RBP: ");
    uint64ToHex(reg->rbp, buffer);
    queueStringStdout(buffer);

    queueStringStdout("   RFLAGS: ");
    uint64ToHex(reg->rflags, buffer);
    queueStringStdout(buffer);
    queueStringStdout("\n");

    queueStringStdout("RAX: ");
    uint64ToHex(reg->rax, buffer);
    queueStringStdout(buffer);

    queueStringStdout("   RBX: ");
    uint64ToHex(reg->rbx, buffer);
    queueStringStdout(buffer);
    queueStringStdout("\n");

    queueStringStdout("RCX: ");
    uint64ToHex(reg->rcx, buffer);
    queueStringStdout(buffer);

    queueStringStdout("   RDX: ");
    uint64ToHex(reg->rdx, buffer);
    queueStringStdout(buffer);
    queueStringStdout("\n");

    queueStringStdout("RSI: ");
    uint64ToHex(reg->rsi, buffer);
    queueStringStdout(buffer);

    queueStringStdout("   RDI: ");
    uint64ToHex(reg->rdi, buffer);
    queueStringStdout(buffer);
    queueStringStdout("\n");
}

void ceroDivHandler(registers_t * reg){
    queueStringStdout("Exception 0: Divide by zero\n");
    loadRegsToStdout(reg);
    // falta volver a shell
}

void invalidInstrucionHandler(registers_t * reg){
    queueStringStdout("Exception 6: Invalid Instruction\n");
    // falta volver a shell
}

static uint64_t system_ticks = 0;  // Contador de ticks global

// Handler del timer (IRQ0)
void timerTickHandler() {
    system_ticks++;  // Incrementa el contador en cada tick
	//drawInt(system_ticks,0xFF0000, 0x000000, 0,0 ,2);
}

uint64_t getSysTicks(){
    return system_ticks;
}



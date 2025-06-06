#include <isrHandlers.h>
#include <stdout.h>
#include <stdint.h>
#include <lib.h>
#include <videoDriver.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OP_EXCEPTION_ID 6

#define SHELL_COLOR 0x00000A
#define FONT_COLOR 0xAAAAAA
#define ERROR_COLOR 0xAA4444
#define PROMPT_COLOR 0x44AAA4
#define FONT_SIZE 2

typedef struct registers{
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

void exceptionDispatcher(int exception, registers_t * regs);
static void buildRegString(char*buff,char*reg,uint64_t value);
static void printRegisters(registers_t * reg);
void timerTickHandler() ;
uint64_t getSysTicks();
static void wait(uint64_t ms);


void exceptionDispatcher(int exception, registers_t * regs) {
    fillScreen(SHELL_COLOR);
    putText("Restart in 5 seconds", ERROR_COLOR, SHELL_COLOR, 20, 20*(18+2), 2); 
    switch (exception){
    case ZERO_EXCEPTION_ID:
        putText("Exception 0: Divide by zero", ERROR_COLOR, SHELL_COLOR, 20, 20*1, 2);
        break;
    case INVALID_OP_EXCEPTION_ID:
        putText("Exception 6: Invalid Instruction", ERROR_COLOR, SHELL_COLOR, 20, 20*1, 2);
        break;
    default:
        break;
    }
    printRegisters(regs);
    // esperar 5 segundos antes de reiniciar
    wait(5000);
}

static void buildRegString(char*buff,char*reg,uint64_t value){
    for (int i = 0 ; i < 8 ; i++){
        buff[i]=reg[i];
    }
    uint64ToHex(value, buff+8);

}

static void printRegisters(registers_t * reg){
    char buffer[28];
    uint64_t regValues[] = { reg->rip, reg->rsp, reg->rbp, reg->rflags, reg->rax, reg->rbx, reg->rcx, reg->rdx,
                             reg->rsi, reg->rdi, reg->r8, reg->r9, reg->r10, reg->r11, reg->r12, reg->r13,
                             reg->r14, reg->r15 };

    char *regNames[] = { "RIP:    ", "RSP:    ", "RBP:    ", "RFLAGS: ", "RAX:    ", "RBX:    ", "RCX:    ", "RDX:    ",
                         "RSI:    ", "RDI:    ", "R8:     ", "R9:     ", "R10:    ", "R11:    ", "R12:    ", "R13:    ",
                         "R14:    ", "R15:    " };

    for (int i = 0; i < 18 ; i++){
        buildRegString(buffer, regNames[i],regValues[i]);
        putText(buffer, FONT_COLOR, SHELL_COLOR, 20, 20*(i+2), 2);
    }
}

static void wait(uint64_t ms){
    uint64_t start = getSysTicks() * 55;
    while (((getSysTicks()*55) - start) < 5000);
}

static uint64_t system_ticks = 0;  // Contador de ticks global
// Handler del timer (IRQ0)
void timerTickHandler() {
    system_ticks++;  // Incrementa el contador en cada tick
}

uint64_t getSysTicks(){
    return system_ticks;
}



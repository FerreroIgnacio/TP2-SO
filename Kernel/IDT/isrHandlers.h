#ifndef ISR_HANDLERS_H
#define ISR_HANDLERS_H
#include <stdint.h>
#include "../scheduler/registerManagement.h"
#define SCHED_QUANTUM_TICKS 10

void ex00Handler();
void zeroDivDispatcher();
void ex06Handler();
void invalidInstrucionDispatcher();
void irq00Handler();
void endIrq00();
void irq01Handler();
void syscallInterruptHandler();
void enableTimerIRQ();
void timerTickHandler(reg_screenshot_t *regs);
reg_screenshot_t *getInteruptRegsBackup(void);
uint64_t getSysTicks();
void timerTickBalanceStack();

#endif

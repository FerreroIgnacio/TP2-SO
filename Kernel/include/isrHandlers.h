#ifndef ISR_HANDLERS_H
#define ISR_HANDLERS_H
#include <stdint.h>
#include <registerManagement.h>
#define SCHED_QUANTUM_TICKS 10

void ex00Handler();
void zeroDivDispatcher();
void ex06Handler();
void invalidInstrucionDispatcher();
void irq00Handler();
void irq01Handler();
void syscallInterruptHandler();
void enableTimerIRQ();
void timerTickHandler(reg_screenshot_t *regs);
uint64_t getSysTicks();

#endif

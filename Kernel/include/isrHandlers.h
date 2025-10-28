#ifndef ISR_HANDLERS_H
#define ISR_HANDLERS_H
#include <stdint.h>
void ex00Handler();
void zeroDivDispatcher();
void ex06Handler();
void invalidInstrucionDispatcher();
void irq00Handler();
void irq01Handler();
void syscallInterruptHandler();
void enableTimerIRQ();
void timerTickHandler();
uint64_t getSysTicks();

#endif

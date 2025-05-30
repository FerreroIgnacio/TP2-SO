#ifndef IDT_INIT_H
#define IDT_INIT_H

#include <stdint.h>
//void IDTadd(uint8_t id, void (*handler)(void), uint8_t flags);
void idtInit();
void idtAdd(uint8_t id, void (*handler)(void), uint8_t flags);
void idtStart();
void enableInterrupts();
#endif

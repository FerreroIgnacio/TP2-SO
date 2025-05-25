#include <idtLoader.h>
#include <videoDriver.h>

#define IDTSIZE 256 

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} IDTEntry;

IDTEntry idtTable[IDTSIZE] = {0}; 

// Funcion para inicializar la tabla en idtLoader.asm
extern void idtStart();

// HANDLERS
extern void irq01Handler();
extern void syscallInterruptHandler();

// Inicializador de la IDT
void idtInit(){
	idtStart();
	idtAdd(0x21, irq01Handler, 0x8E);
	idtAdd(0x80, syscallInterruptHandler, 0x8E); 
}

/*
 *	Agrega una funcion declarada en asm que no toma ningun parametro de llamada (importante) a la IDT
 *	@param id indice de la interrupcion
 *	@flags flags
 */
void idtAdd(uint8_t id, void (*handler)(void), uint8_t flags) {
    if (id >= IDTSIZE) return;

    uint64_t addr = (uint64_t)handler;

    IDTEntry entry = {
        .offset_low = addr & 0xFFFF,
        .selector = 0x08,
        .ist = 0,
        .flags = flags,
        .offset_mid = (addr >> 16) & 0xFFFF,
        .offset_high = (addr >> 32) & 0xFFFFFFFF,
        .zero = 0
    };
    idtTable[id] = entry;
}

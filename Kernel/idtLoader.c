#include <idtLoader.h>

#define IDTSIZE 256 

typedef struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) IDTEntry;

static IDTEntry IDTTable[IDTSIZE] = {0}; 
int IDTinited = 0;

//funcion para inicializar la tabla en idtLoader.asm
extern void IDTinit(void);

/*
 *	Agrega una funcion declarada en asm que no toma ningun parametro de llamada (importante) a la IDT
 *	@param id indice de la interrupcion
 *	@flags flags
 */
void IDTadd(uint8_t id, void (*handler)(void), uint8_t flags) {
    if (!IDTinited) {
        IDTinit();
        IDTinited = 1;
    }

    if (id >= IDTSIZE) return;

    uint64_t addr = (uint64_t)handler;

    struct IDTEntry entry = {
        .offset_low = addr & 0xFFFF,
        .selector = 0x08,
        .ist = 0,
        .flags = flags,
        .offset_mid = (addr >> 16) & 0xFFFF,
        .offset_high = (addr >> 32) & 0xFFFFFFFF,
        .zero = 0
    };

    IDTTable[id] = entry;
}

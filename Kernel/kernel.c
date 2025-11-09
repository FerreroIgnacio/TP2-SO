#include <stdint.h>
#include <string.h>
#include "utils/lib.h"
#include "kernelStart/moduleLoader.h"
#include "kernelStart/naiveConsole.h"
#include "videoDriver/videoDriver.h"
#include "keyboardDriver/keyboardDriver.h"
#include "IDT/idtInit.h"
#include "filesDescriptors/stdout.h"
#include "memoryManagement/mm.h"
#include "scheduler/scheduler.h"
#include "scheduler/cpu.h"
#include "filesDescriptors/fd.h" // added

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;
static const uint64_t StackPages = 8;
static const uintptr_t HeapAlignment = 16;

extern void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

extern char font8x8_basic[128][8];

static void *const shellModuleAddress = (void *)0x7000000;      // Shell
static void *const pongisgolfModuleAddress = (void *)0x8000000; // PongisGolf

typedef int (*EntryPoint)();

static void initialize_memory_manager(void)
{
    // Arranco el heap justo después del kernel y su stack
    uint8_t *heap_start = (uint8_t *)&endOfKernel + (PageSize * StackPages);
    // Alineo el inicio del heap
    uintptr_t aligned_start = ((uintptr_t)heap_start + (HeapAlignment - 1)) & ~(HeapAlignment - 1);
    // Termino el heap al inicio del siguiente módulo cargado (Shell)
    uint8_t *heap_end = (uint8_t *)shellModuleAddress;

    if (heap_end > (uint8_t *)aligned_start)
    {
        mm_init((void *)aligned_start, (size_t)(heap_end - (uint8_t *)aligned_start));
    }
}

void clearBSS(void *bssAddress, uint64_t bssSize)
{
    memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
    return (void *)((uint64_t)&endOfKernel + PageSize * StackPages // The size of the stack itself, 32KiB
                    - sizeof(uint64_t)                             // Begin at the top of the stack
    );
}

void *initializeKernelBinary()
{
    void *moduleAddresses[] = {
        shellModuleAddress,     // [0] -> Shell va a 0x7000000
        pongisgolfModuleAddress // [2] -> PongisGolf va a 0x8000000
    };

    loadModules(&endOfKernelBinary, moduleAddresses);
    clearBSS(&bss, &endOfKernel - &bss);
    return getStackBase();
}
extern void pic_init();

int main()
{
    initialize_memory_manager();
    idtInit();
    fd_init();

    scheduler_add((task_fn_t)shellModuleAddress, NULL);
    scheduler_start();
    while (1)
        ;
}

int __stack_chk_fail(void)
{
    // Retorna -13333 cuando se detecta un error de stack
    return -13333;
}

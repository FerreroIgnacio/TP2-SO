#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <idtInit.h>
#include <stdout.h>
#include <mm.h>
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

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const pongisgolfModuleAddress = (void*)0x600000;

typedef int (*EntryPoint)();

static void initialize_memory_manager(void) {
	uint8_t *heap_start = (uint8_t *)&endOfKernel + (PageSize * StackPages);
	uintptr_t aligned_start = ((uintptr_t)heap_start + (HeapAlignment - 1)) & ~(HeapAlignment - 1);
	uint8_t *heap_end = (uint8_t *)sampleCodeModuleAddress;

	if (heap_end > (uint8_t *)aligned_start) {
		mm_init((void *)aligned_start, (size_t)(heap_end - (uint8_t *)aligned_start));
	}
}


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * StackPages		//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
    sampleCodeModuleAddress,     // [0] -> Shell goes to 0x400000
    sampleDataModuleAddress,     // [1] -> Data goes to 0x500000 
    pongisgolfModuleAddress      // [2] -> PongisGolf goes to 0x600000
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}
extern void pic_init();
int main()
{
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncPrint("  Calling the sample code module returned: ");
//	ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	ncNewline();
	ncNewline();

	ncPrint("  Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();

	ncPrint("[Finished]");
	

	initialize_memory_manager();
	idtInit();
	((EntryPoint)sampleCodeModuleAddress)();
	
	while(1);
	return 0;
}
int __stack_chk_fail(void) {
    // Retorna -13333 cuando se detecta un error de stack
    return -13333;
}

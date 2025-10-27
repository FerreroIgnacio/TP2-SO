include Makefile.inc
KERNEL = kernel.bin
SOURCES = $(wildcard *.c) $(wildcard mm/*.c)
SOURCES_ASM = $(filter-out asm/loader.asm, $(wildcard asm/*.asm))

OBJECTS = $(SOURCES:.c=.o)
OBJECTS_ASM = $(SOURCES_ASM:.asm=.o)
LOADEROBJECT = asm/loader.o
STATICLIBS =

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

asm/%.o: asm/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

asm/loader.o: asm/loader.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f *.o asm/*.o mm/*.o *.bin

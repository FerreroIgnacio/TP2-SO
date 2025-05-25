include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)

all: $(MODULE)

$(MODULE): $(SOURCES) ../standard.c ../standard.asm testFn.asm _loader.c
	$(ASM) $(ASMFLAGS) -o standard_asm.o ../standard.asm
	$(ASM) $(ASMFLAGS) -o testFn.o testFn.asm
	$(GCC) $(GCCFLAGS) -c ../standard.c -o standard_c.o
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) standard_c.o standard_asm.o testFn.o -o ../$(MODULE)

clean:
	rm -rf *.o ../*.o

.PHONY: all clean

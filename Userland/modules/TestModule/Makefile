include ../Makefile.inc

MODULE=0003-TestModule.bin
SOURCES=$(wildcard [^_]*.c)

all: $(MODULE)

$(MODULE): $(SOURCES) ../standard.c ../standard.asm _loader.c ../fontManager.c
	$(ASM) $(ASMFLAGS) -o standard_asm.o ../standard.asm
	$(GCC) $(GCCFLAGS) -c ../standard.c -o standard_c.o
	$(GCC) $(GCCFLAGS) -I../include -c ../fontManager.c -o fontManager.o
	$(GCC) $(GCCFLAGS) -T testmodule.ld _loader.c $(SOURCES) standard_c.o standard_asm.o fontManager.o -o ../$(MODULE)

clean:
	rm -rf *.o ../*.o

.PHONY: all clean

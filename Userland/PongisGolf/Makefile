include ../Makefile.inc

MODULE=0002-PongisGolf.bin
SOURCES=$(wildcard [^_]*.c)

all: $(MODULE)

$(MODULE): $(SOURCES) ../standard.c ../standard.asm _loader.c
	$(ASM) $(ASMFLAGS) -o standard_asm.o ../standard.asm
	$(GCC) $(GCCFLAGS) -c ../standard.c -o standard_c.o
	$(GCC) $(GCCFLAGS) -T pongisgolf.ld _loader.c $(SOURCES) standard_c.o standard_asm.o -o ../$(MODULE)

clean:
	rm -rf *.o ../*.o

.PHONY: all clean
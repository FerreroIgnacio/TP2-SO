include ../Makefile.inc

MODULE=0000-Shell.bin
SOURCES=$(wildcard [^_]*.c)
HEADERS=$(wildcard *.h) $(wildcard ../include/*.h)
all: $(MODULE)

$(MODULE): $(SOURCES) $(HEADERS) ../standard.c ../standard.asm _loader.c ../fontManager.c
	$(ASM) $(ASMFLAGS) -o standard_asm.o ../standard.asm
	$(GCC) $(GCCFLAGS) -I../include -c ../standard.c -o standard_c.o
	$(GCC) $(GCCFLAGS) -I../include -c ../fontManager.c -o fontManager.o
	$(GCC) $(GCCFLAGS) -I../include -T shell.ld _loader.c $(SOURCES) standard_c.o standard_asm.o  fontManager.o -o ../$(MODULE)

clean:
	rm -rf *.o ../*.o

.PHONY: all clean

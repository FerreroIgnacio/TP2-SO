include ../Makefile.inc

# Carpetas de librerías
SUBDIRS := standard video fontManager invalidOpCode test

# Archivos fuente
CSOURCES := $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
ASMSOURCES := $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.asm))

# Objetos: .c -> .o y .asm -> _asm.o
C_OBJS := $(CSOURCES:.c=.o)
ASM_OBJS := $(ASMSOURCES:.asm=_asm.o)

all: $(C_OBJS) $(ASM_OBJS)

# Compilación de C
standard/%.o: standard/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

video/%.o: video/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

fontManager/%.o: fontManager/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

invalidOpCode/%.o: invalidOpCode/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

test/%.o: test/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

# Compilación de ASM con sufijo _asm
standard/%_asm.o: standard/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

video/%_asm.o: video/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

fontManager/%_asm.o: fontManager/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

invalidOpCode/%_asm.o: invalidOpCode/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	rm -f $(C_OBJS) $(ASM_OBJS)

.PHONY: all clean
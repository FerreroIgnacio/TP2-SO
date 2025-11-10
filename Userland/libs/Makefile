include ../Makefile.inc

# Carpetas de librerías
SUBDIRS := standard video fontManager invalidOpCode test mystring audio fileDescriptorUtils memory keyboard time

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

mystring/%.o: mystring/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

audio/%.o: audio/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

fileDescriptorUtils/%.o: fileDescriptorUtils/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

memory/%.o: memory/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

keyboard/%.o: keyboard/%.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

time/%.o: time/%.c
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

audio/%_asm.o: audio/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

fileDescriptorUtils/%_asm.o: fileDescriptorUtils/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

memory/%_asm.o: memory/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

keyboard/%_asm.o: keyboard/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

time/%_asm.o: time/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	rm -f $(C_OBJS) $(ASM_OBJS)

.PHONY: all clean
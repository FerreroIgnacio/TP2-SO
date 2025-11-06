include Makefile.inc

KERNEL = kernel.bin
OBJDIR = obj

# Busca todos los .c recursivamente, EXCLUYENDO archivos problemáticos
SOURCES := $(filter-out ./videoDriver/font8x8/render.c, $(shell find . -type f -name "*.c"))

# Busca todos los .asm (excepto loader.asm que se maneja aparte)
SOURCES_ASM := $(filter-out ./kernelStart/loader.asm, $(shell find . -type f -name "*.asm"))

# Convierte rutas fuente a rutas en ./obj
OBJECTS := $(patsubst ./%, $(OBJDIR)/%, $(SOURCES:.c=.o))
# Para ASM: agregar sufijo _asm antes de .o para evitar colisiones
OBJECTS_ASM := $(patsubst ./%, $(OBJDIR)/%, $(SOURCES_ASM:.asm=_asm.o))

LOADEROBJECT := $(OBJDIR)/kernelStart/loader.o

STATICLIBS =

# Target para debug: ver qué archivos encuentra
debug:
	@echo "=== C Sources ==="
	@echo $(SOURCES)
	@echo ""
	@echo "=== ASM Sources ==="
	@echo $(SOURCES_ASM)
	@echo ""
	@echo "=== C Objects ==="
	@echo $(OBJECTS)
	@echo ""
	@echo "=== ASM Objects ==="
	@echo $(OBJECTS_ASM)

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^

# Regla genérica para .c → .o
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

# Regla genérica para .asm → _asm.o (sufijo para evitar conflictos)
$(OBJDIR)/%_asm.o: %.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

# Loader especial
$(OBJDIR)/kernelStart/loader.o: kernelStart/loader.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(OBJDIR) *.bin

.PHONY: all clean debug
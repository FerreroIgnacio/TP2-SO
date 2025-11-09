include ../../Makefile.inc

MODULE = 0000-Shell.bin

# Archivos fuente locales (ignora archivos que empiezan con "_")
SOURCES := $(wildcard *.c)
OBJS := $(SOURCES:.c=.o)

# Carpeta de librerías externas
LIBS_DIR = ../../libs

all: $(MODULE)

# Build libs first, then link with fresh list of lib objects
$(MODULE): $(OBJS)
	$(MAKE) -C $(LIBS_DIR) all
	$(GCC) $(GCCFLAGS) -T shell.ld $(OBJS) $(shell find $(LIBS_DIR) -name "*.o") -o ../$(MODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

clean:
	rm -f *.o ../$(MODULE)

.PHONY: all clean

include ../../Makefile.inc

MODULE = 0003-TestModule.bin

# Archivos fuente locales (ignora archivos que empiezan con "_")
SOURCES := $(wildcard [^_]*.c)
OBJS := $(SOURCES:.c=.o)

# Carpeta de librerías externas
LIBS_DIR = ../../libs
# Todos los .o dentro de libs y sus subcarpetas
LIB_OBJS := $(shell find $(LIBS_DIR) -name "*.o")

all: $(MODULE)

$(MODULE): $(OBJS) $(LIB_OBJS)
	$(GCC) $(GCCFLAGS) -T testmodule.ld $(OBJS) $(LIB_OBJS) -o ../$(MODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I../include -c $< -o $@

clean:
	rm -f *.o ../$(MODULE)

.PHONY: all clean


include Makefile.inc

SAMPLE_DATA=0001-Shell.bin

all: Shell PongisGolf shell

Shell:
	cd Shell; make

PongisGolf:
	cd PongisGolf; make

shell:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd Shell; make clean
	cd PongisGolf; make clean
	rm -rf *.bin

.PHONY: Shell PongisGolf all clean
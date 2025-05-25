include Makefile.inc

SAMPLE_DATA=0001-Shell.bin

all: Shell shell

Shell:
	cd Shell; make

shell:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd Shell; make clean
	rm -rf *.bin


.PHONY: Shell all clean

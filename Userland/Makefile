include Makefile.inc

SAMPLE_DATA=0001-Shell.bin

all: Libs Shell PongisGolf TestModule

Shell:
	cd modules/Shell; make

PongisGolf:
	cd modules/PongisGolf; make

TestModule:
	cd modules/TestModule; make all
Libs:
	cd libs; make
shell:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd modules/Shell; make clean
	cd modules/PongisGolf; make clean
	cd modules/TestModule; make clean
	rm -rf *.bin

.PHONY: Shell PongisGolf all clean
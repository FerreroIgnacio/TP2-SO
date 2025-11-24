include Makefile.inc

SAMPLE_DATA=0000-Shell.bin

all: Libs Shell

Shell:
	cd modules/Shell; make

PongisGolf:
	cd modules/PongisGolf; make

TestModule:
	cd modules/TestModule; make all
Libs:
	cd libs; make

clean:
	cd modules/Shell; make clean
	cd modules/PongisGolf; make clean
	cd modules/TestModule; make clean
	cd libs; make clean
	rm -rf *.bin

.PHONY: Shell all clean
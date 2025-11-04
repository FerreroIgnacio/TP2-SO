
ifdef USE_BUDDY
KERNEL_MAKEFLAGS:=USE_BUDDY=$(USE_BUDDY)
endif

all:  bootloader kernel userland image

bootloader:
	cd Bootloader; $(MAKE) all

kernel:
	cd Kernel; $(MAKE) $(KERNEL_MAKEFLAGS) all

userland:
	cd Userland; $(MAKE) all

image: kernel bootloader userland
	cd Image; $(MAKE) all

buddy:
	$(MAKE) USE_BUDDY=1 all

clean:
	cd Bootloader; $(MAKE) clean
	cd Image; $(MAKE) clean
	cd Kernel; $(MAKE) clean
	cd Userland; $(MAKE) clean

.PHONY: bootloader image collections kernel userland all clean buddy

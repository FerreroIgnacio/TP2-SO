ifdef USE_BUDDY
KERNEL_MAKEFLAGS:=USE_BUDDY=$(USE_BUDDY)
endif

# Detectar el sistema operativo
UNAME_S := $(shell uname -s)

# Configuración de X11 según el OS
ifeq ($(UNAME_S),Linux)
    DOCKER_DISPLAY := -e DISPLAY=$(DISPLAY) -v /tmp/.X11-unix:/tmp/.X11-unix
    XHOST_CMD := @xhost +local:docker > /dev/null 2>&1 || true
else ifeq ($(UNAME_S),Darwin)
    # macOS - necesitas XQuartz instalado y corriendo
    DOCKER_DISPLAY := -e DISPLAY=host.docker.internal:0
    XHOST_CMD := @xhost + 127.0.0.1 > /dev/null 2>&1 || true
else
    # Windows con WSL2
    DOCKER_DISPLAY := -e DISPLAY=host.docker.internal:0
    XHOST_CMD := @echo "Asegúrate de tener un servidor X11 corriendo (VcXsrv o similar)"
endif

all:  toolchain bootloader kernel userland image

toolchain:
	cd Toolchain; $(MAKE) all

bootloader:
	cd Bootloader; $(MAKE) all

kernel:
	cd Kernel; $(MAKE) $(KERNEL_MAKEFLAGS) all

userland:
	cd Userland; $(MAKE) all

image: kernel bootloader userland toolchain
	cd Image; $(MAKE) all

buddy:
	$(MAKE) USE_BUDDY=1 all

clean:
	cd Bootloader; $(MAKE) clean
	cd Image; $(MAKE) clean
	cd Kernel; $(MAKE) clean
	cd Userland; $(MAKE) clean
	cd Toolchain; $(MAKE) clean

docker-pull:
	docker pull agodio/itba-so-multi-platform:3.0

docker-run:
	$(XHOST_CMD)
	docker run -it --rm \
		$(DOCKER_DISPLAY) \
		-v $(PWD):/root/workspace \
		agodio/itba-so-multi-platform:3.0 \
		/bin/bash -c "cd /root/workspace && exec bash"

docker-display:
	$(XHOST_CMD)
	docker run -it --rm \
		$(DOCKER_DISPLAY) \
		-v $(PWD):/root/workspace \
		agodio/itba-so-multi-platform:3.0 \
		/bin/bash -c "cd /root/workspace && qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512M"

.PHONY: bootloader image collections kernel userland toolchain all clean buddy docker-pull docker-run docker-display
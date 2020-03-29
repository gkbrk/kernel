export PATH := /i686-elf/bin:$(PATH)
C_FILES = $(shell find ./kernel/ -type f -name '*.c')
OBJ_FILES = $(patsubst ./%.c, build/%.o, $(C_FILES)) build/kernel.o

CPP_FILES = $(shell find . -type f -name '*.cpp')
OBJ_FILES += $(patsubst ./%.cpp, build/%.o, $(CPP_FILES))

CC = i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -Os -flto
CFLAGS += -Wall -Wextra -pedantic

CXX = i686-elf-c++
CXXFLAGS = -Os -flto
CXXFLAGS += -Wall -Wextra -fno-exceptions -fno-rtti -ffreestanding

.INTERMEDIATE: $(OBJ_FILES)

all: build/leonardo.bin
.PHONY: all

build/leonardo.bin: $(OBJ_FILES) build/switchTask.o build/boot.o
	$(CC) -T linker.ld -flto -nostdlib -Os -o "$@" $^ -lgcc

build/boot.o: boot.asm
	mkdir -p "$(@D)"
	nasm -felf32 $< -o "$@"

build/switchTask.o: kernel/switchTask.s
	mkdir -p "$(@D)"
	as --32 -c "$<" -o "$@"

build/kernel.o: kernel.c
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -I. -c "$<" -o "$@"


build/%.o: %.c
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -I. -c "$<" -o "$@"

build/%.o: %.cpp
	mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) -I. -c "$<" -o "$@"

clean:
	@rm -rf build
.PHONY: clean

tar-drive.tar: $(shell find tar-drive/ -type f)
	cd tar-drive; tar cvf ../tar-drive.tar *

run: all tar-drive.tar
	qemu-system-x86_64 -serial mon:stdio -kernel build/leonardo.bin -hda tar-drive.tar -d guest_errors -soundhw pcspk
.PHONY: run

iso: all
	mkdir -p build/iso/boot/grub
	cp build/leonardo.bin build/iso/boot/leonardo.bin
	echo "menuentry \"leonardo\" { multiboot /boot/leonardo.bin }" > build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/iso/leonardo.iso build/iso
.PHONY: iso

format:
	clang-format -i $(shell find kernel/ -type f -name '*.c')
	clang-format -i $(shell find kernel/ -type f -name '*.cpp')
	clang-format -i $(shell find kernel/ -type f -name '*.h')
.PHONY: format

reload:
	find . -type f -name '*.c' -or -name '*.cpp' -or -name '*.h' > QtCreator.files
.PHONY: reload

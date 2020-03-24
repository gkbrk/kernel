export PATH := /i686-elf/bin:$(PATH)
C_FILES = $(shell find kernel/ -type f -name '*.c')
OBJ_FILES = $(patsubst kernel/%.c, build/%.o, $(C_FILES)) build/kernel.o

CC = i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -Os -flto
CFLAGS += -Wall -Wextra -pedantic

all: build/leonardo.bin

build/leonardo.bin: $(OBJ_FILES) build/switchTask.o build/kernel.o build/boot.o
	$(CC) -T linker.ld -nostdlib -o "$@" $(CFLAGS) $^ -lgcc

build/boot.o: boot.asm
	mkdir -p "$(@D)"
	nasm -felf32 $< -o "$@"

build/switchTask.o: kernel/switchTask.s
	mkdir -p "$(@D)"
	as --32 -c "$<" -o "$@"

build/kernel.o: kernel.c
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -I. -c "$<" -o "$@"


build/%.o: kernel/%.c
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -I. -c "$<" -o "$@"

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
	clang-format -i $(shell find kernel/ -type f -name '*.h')
.PHONY: format

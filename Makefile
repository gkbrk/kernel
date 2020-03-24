export PATH := /i686-elf/bin:$(PATH)
C_FILES = $(shell find kernel/ -type f -name '*.c')
OBJ_FILES = $(patsubst kernel/%.c, build/%.o, $(C_FILES)) build/kernel.o

CC = i686-elf-gcc
CFLAGS=-c -I. -std=gnu99 -ffreestanding -Os -flto
#CFLAGS += -Wall -Wextra -pedantic

all: clean $(OBJ_FILES)
	nasm -felf32 boot.asm -o build/boot.o
	as --32 -c kernel/switchTask.s -o build/switchTask.o
	i686-elf-gcc -T linker.ld -o build/leonardo.bin -flto -ffreestanding -Os -nostdlib build/switchTask.o build/boot.o $(OBJ_FILES) -lgcc

build/%.o: kernel/%.c
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) "$<" -o "$@"

build/kernel.o: kernel.c
	$(CC) $(CFLAGS) "$<" -o "$@"

clean:
	rm -rfv build

run:
	make all
	cd tar-drive; tar cvf ../tar-drive.tar *
	qemu-system-x86_64 -serial mon:stdio -kernel build/leonardo.bin -hda tar-drive.tar -d guest_errors -soundhw pcspk

iso:
	make all
	mkdir -p build/iso/boot/grub
	cp build/leonardo.bin build/iso/boot/leonardo.bin
	echo "menuentry \"leonardo\" { multiboot /boot/leonardo.bin }" > build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/iso/leonardo.iso build/iso

format:
	clang-format -i $(shell find kernel/ -type f -name '*.c')
	clang-format -i $(shell find kernel/ -type f -name '*.h')
.PHONY: format
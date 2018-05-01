export PATH := /i686-elf/bin:$(PATH)

all:
	make clean
	mkdir build
	nasm -felf32 boot.asm -o build/boot.o
	as -c kernel/switchTask.s -o build/switchTask.o
	i686-elf-gcc -c kernel.c -o build/kernel.o -I. -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -T linker.ld -o build/leonardo.bin -ffreestanding -O2 -nostdlib build/switchTask.o build/boot.o build/kernel.o -lgcc

clean:
	rm -rfv build

run:
	make all
	qemu-system-x86_64 -kernel build/leonardo.bin

iso:
	make all
	mkdir -p build/iso/boot/grub
	cp build/leonardo.bin build/iso/boot/leonardo.bin
	echo "menuentry \"leonardo\" { multiboot /boot/leonardo.bin }" > build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/iso/leonardo.iso build/iso
#!/bin/sh
set -e

# Bootloader
nasm -f bin boot.asm -o boot.bin

# Kernel ASM
nasm -f elf64 pm.asm -o pm.o
nasm -f elf64 idt.asm -o idt.o

# Kernel C
gcc -ffreestanding -m64 -mno-red-zone -fno-pic \
    -fno-stack-protector \
    -c kernel.c -o kernel.o

gcc -ffreestanding -m64 -mno-red-zone -fno-pic \
    -fno-stack-protector \
    -c idt.c -o idt_c.o

# Link
ld -nostdlib -T linker.ld \
   pm.o kernel.o idt.o idt_c.o \
   -o kernel.bin --oformat binary

# Disk image
dd if=/dev/zero of=os.img bs=512 count=2048
dd if=boot.bin of=os.img bs=512 seek=0 conv=notrunc
dd if=kernel.bin of=os.img bs=512 seek=1 conv=notrunc

# Run
qemu-system-x86_64 \
  -drive format=raw,file=os.img \
  -no-reboot -d int

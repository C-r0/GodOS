#!/bin/sh
set -e

ROOT=$(dirname "$0")/..
SRC=$ROOT/src
BUILD=$ROOT/build

mkdir -p "$BUILD"

echo "[*] Assembling bootloader..."
nasm -f bin "$SRC/boot/boot.asm" -o "$BUILD/boot.bin"

echo "[*] Assembling kernel ASM..."
nasm -f elf64 "$SRC/arch/pm.asm" -o "$BUILD/pm.o"
nasm -f elf64 "$SRC/kernel/idt.asm" -o "$BUILD/idt.o"

echo "[*] Compiling kernel C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/kernel.c" -o "$BUILD/kernel.o"

gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/idt.c" -o "$BUILD/idt_c.o"

gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/vga.c" -o "$BUILD/vga.o"
    
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/shell.c" -o "$BUILD/shell.o"
    
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/string.c" -o "$BUILD/string.o"
    
echo "[*] Assembling IO ASM..."
nasm -f elf64 "$SRC/kernel/io.asm" -o "$BUILD/io.o"

echo "[*] Compiling keyboard..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/keyboard.c" -o "$BUILD/keyboard.o"
    
echo "[*] Compiling PIC..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/pic.c" -o "$BUILD/pic.o"
    
echo "[*] Assembling ISRs..."
nasm -f elf64 "$SRC/kernel/isr.asm" -o "$BUILD/isr.o"

echo "[*] Linking kernel..."
ld -nostdlib -T "$SRC/linker.ld" \
   "$BUILD/pm.o" \
   "$BUILD/kernel.o" \
   "$BUILD/idt.o" \
   "$BUILD/idt_c.o" \
   "$BUILD/vga.o" \
   "$BUILD/shell.o" \
   "$BUILD/keyboard.o" \
   "$BUILD/pic.o" \
   "$BUILD/isr.o" \
   "$BUILD/io.o" \
   "$BUILD/string.o" \
   -o "$BUILD/kernel.bin" --oformat binary


echo "[*] Creating disk image..."
dd if=/dev/zero of="$BUILD/os.img" bs=512 count=2048 status=none
dd if="$BUILD/boot.bin" of="$BUILD/os.img" bs=512 seek=0 conv=notrunc status=none
dd if="$BUILD/kernel.bin" of="$BUILD/os.img" bs=512 seek=1 conv=notrunc status=none

echo "[*] Running QEMU..."
qemu-system-x86_64 -drive format=raw,file="$BUILD/os.img" -no-reboot -m 512M

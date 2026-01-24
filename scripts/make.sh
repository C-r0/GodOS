#!/bin/sh
set -e

ROOT=$(dirname "$0")/..
SRC=$ROOT/src
BUILD=$ROOT/build

mkdir -p "$BUILD"

echo "[*] Assembling bootloader..."
nasm -f bin "$SRC/boot/boot.asm" -o "$BUILD/boot.bin"

echo "[*] Assembling kernel ASM..."
nasm -f elf64 "$SRC/kernel/pm.asm" -o "$BUILD/pm.o"
nasm -f elf64 "$SRC/kernel/idt.asm" -o "$BUILD/idt.o"

echo "[*] Compiling kernel C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/kernel.c" -o "$BUILD/kernel.o"

gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/idt.c" -o "$BUILD/idt_c.o"

echo "[*] Linking kernel..."
ld -nostdlib -T "$SRC/linker.ld" \
   "$BUILD/pm.o" \
   "$BUILD/kernel.o" \
   "$BUILD/idt.o" \
   "$BUILD/idt_c.o" \
   -o "$BUILD/kernel.bin" --oformat binary

echo "[*] Creating disk image..."
dd if=/dev/zero of="$BUILD/os.img" bs=512 count=2048 status=none
dd if="$BUILD/boot.bin" of="$BUILD/os.img" bs=512 seek=0 conv=notrunc status=none
dd if="$BUILD/kernel.bin" of="$BUILD/os.img" bs=512 seek=1 conv=notrunc status=none

echo "[*] Running QEMU..."
qemu-system-x86_64 \
  -drive format=raw,file="$BUILD/os.img" \
  -no-reboot -d int

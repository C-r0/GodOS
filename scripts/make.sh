#!/bin/sh
set -e

ROOT=$(dirname "$0")/..
SRC=$ROOT/fallback/src
BUILD=$ROOT/build

mkdir -p "$BUILD"

echo "[*] Compiling Bootloader..."
echo "[-] Compiling Boot ASM..."
nasm -f bin "$SRC/boot/boot.asm" -o "$BUILD/boot.bin"
echo "[+] Compiled Boot ASM..."
echo "[-] Compiling PM ASM..."
nasm -f elf64 "$SRC/arch/pm.asm" -o "$BUILD/pm.o"
echo "[+] Compiled PM ASM..."

echo "----- BOOTLOADER COMPLETE -----"

echo "[*] Compiling Kernel..."
echo "[-] Compiling Kernel C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/KERNEL/kernel.c" -o "$BUILD/kernel.o"
echo "[+] Compiled Kernel C..."
echo "[-] Compiling Shell C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/KERNEL/shell.c" -o "$BUILD/shell.o"
echo "[+] Compiled Shell C..."

echo "----- KERNEL COMPLETE -----"

echo "[*] Compiling IDT..."
echo "[-] Compiling IDT asm..."
nasm -f elf64 "$SRC/kernel/IDT/idt.asm" -o "$BUILD/idt.o"
echo "[+] Compiled IDT asm..."
echo "[-] Compiling IDT C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/IDT/idt.c" -o "$BUILD/idt_c.o"
echo "[+] Compiled IDT C..."
echo "[-] Assembling IO ASM..."
nasm -f elf64 "$SRC/kernel/IDT/io.asm" -o "$BUILD/io.o"
echo "[+] Assembled IO ASM..."
echo "[-] Assembling ISRs..."
nasm -f elf64 "$SRC/kernel/IDT/isr.asm" -o "$BUILD/isr.o"
echo "[+] Assembled ISRs..."

echo "----- IDT COMPLETE -----"

echo "[*] Compiling Memory..."
echo "[-] Compiling PMM C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/MEMORY/pmm.c" -o "$BUILD/pmm.o"
echo "[+] Compiled PMM C..."

echo "----- MEMORY COMPLETE -----"

echo "[*] Compiling PIC..."
echo "[-] Compiling PIC C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/PIC/pic.c" -o "$BUILD/pic.o"
echo "[+] Compiled PIC C..."

echo "----- PIC COMPLETE -----"

echo "[*] Compiling Driver..."
echo "[-] Compiling Keyboard C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/DRIVER/keyboard.c" -o "$BUILD/keyboard.o"
echo "[+] Compiled Keyboard C..."
echo "[-] Compiling Timer C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/DRIVER/timer.c" -o "$BUILD/timer.o"
echo "[+] Compiled Timer C..."
echo "[-] Compiling Vga C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/DRIVER/vga.c" -o "$BUILD/vga.o"
echo "[+] Compiled Vga C..."

echo "----- DRIVER COMPLETE -----"

echo "[*] Compiling LIB..."
echo "[-] Compiling String C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/LIB/string.c" -o "$BUILD/string.o"
echo "[+] Compiled String C..."
echo "[-] Compiling Math C..."
gcc -ffreestanding -m64 -mno-red-zone -fno-pic -fno-stack-protector \
    -c "$SRC/kernel/LIB/math.c" -o "$BUILD/math.o"
echo "[+] Compiled Math C..."

echo "----- LIB COMPLETE -----"

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
   "$BUILD/pmm.o" \
   "$BUILD/math.o" \
   "$BUILD/timer.o" \
   -o "$BUILD/kernel.bin" --oformat binary

echo "[*] Creating disk image..."
dd if=/dev/zero of="$BUILD/os.img" bs=512 count=8192 status=none
dd if="$BUILD/boot.bin" of="$BUILD/os.img" bs=512 seek=0 conv=notrunc status=none
dd if="$BUILD/kernel.bin" of="$BUILD/os.img" bs=512 seek=1 conv=notrunc status=none

echo "[*] Running QEMU..."
qemu-system-x86_64 -drive format=raw,file=build/os.img -m 512M -accel tcg,thread=multi -cpu max

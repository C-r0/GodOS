#include "idt.h"

void kernel_main(void) {
    idt_init();

    volatile char *vga = (volatile char*)0xB8000;
    vga[0] = 'I';
    vga[1] = 0x0F;
    vga[2] = 'D';
    vga[3] = 0x0F;
    vga[4] = 'T';
    vga[5] = 0x0F;

    for (;;);
}

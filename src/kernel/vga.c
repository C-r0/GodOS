#include <stdint.h>

static volatile uint16_t *vga = (volatile uint16_t*)0xB8000;
static int cursor = 0;

void vga_clear(void) {
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0F20; // espaço branco
    }
    cursor = 0;
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor += 80 - (cursor % 80);
        return;
    }

    vga[cursor++] = (uint16_t)c | 0x0F00;
}

void vga_print(const char *str) {
    while (*str) {
        vga_putc(*str++);
    }
}

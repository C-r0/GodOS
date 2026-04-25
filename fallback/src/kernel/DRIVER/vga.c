#include <stdint.h>
#include "../include/io.h"

static volatile uint16_t *vga = (volatile uint16_t*)0xB8000;
static int cursor = 0;

uint8_t g_current_color = 0x0F; 

void vga_set_color(uint8_t color) {
    g_current_color = color;
}

void update_hardware_cursor() {
    uint16_t pos = cursor;
    if (pos < 0) pos = 0; // Proteção contra o -80
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_scroll() {
    for (int i = 0; i < (24 * 80); i++) {
        vga[i] = vga[i + 80];
    }

    uint16_t blank = (uint16_t)' ' | (uint16_t)g_current_color << 8;
    for (int i = 24 * 80; i < 25 * 80; i++) {
        vga[i] = blank;
    }

    cursor = 1920;
}

void vga_clear() {
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0F20; 
    }
    cursor = 0;
}

void vga_putc(char c, uint8_t cb) {
    if (c == '\n') {
        cursor += 80 - (cursor % 80);
    } else if (c == '\b'){
        if (cursor > 0) {
            cursor--;
            vga[cursor] = (uint16_t)' ' | (uint16_t)0x07 << 8; 
        }
    } else {
        vga[cursor++] = (uint16_t)c | ((uint16_t)cb << 8);
    }

    if (cursor >= 2000) {
        vga_scroll();
        cursor = 1920;
    }
    update_hardware_cursor();
}

void vga_print(const char *str, uint8_t cb) {
    while (*str) {
        vga_putc(*str++, cb);
    }
}

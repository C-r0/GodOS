#pragma once

#include <stdint.h>

void vga_clear();
void vga_putc(char c, uint8_t cb);
void vga_print(const char *str, uint8_t cb);
void update_hardware_cursor();

extern uint8_t g_current_color; 

void vga_set_color(uint8_t color);

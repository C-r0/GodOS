#include "../include/timer.h"
#include "../include/vga.h"

volatile uint64_t g_ticks = 0;

void timer_callback(void) {
    g_ticks++;
}

void timer_init(void) {}

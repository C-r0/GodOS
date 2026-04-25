#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint64_t g_ticks;

void timer_init(void);
void timer_callback(void);

#endif

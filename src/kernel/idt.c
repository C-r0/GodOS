#include "idt.h"

extern void isr_default(void);

static struct idt_entry idt[256];

static void idt_set_gate(int n, uint64_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08; // code segment
    idt[n].ist         = 0;
    idt[n].type_attr   = 0x8E; // interrupt gate
    idt[n].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].zero        = 0;
}

void idt_init(void) {
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint64_t)isr_default);
    }

    struct idt_ptr idtr = {
        .limit = sizeof(idt) - 1,
        .base  = (uint64_t)&idt
    };

    asm volatile ("lidt %0" : : "m"(idtr));
    asm volatile ("sti");
}

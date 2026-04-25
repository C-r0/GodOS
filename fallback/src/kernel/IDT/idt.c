#include "../include/idt.h"
#include "../include/pic.h"
#include "../include/io.h"

extern void isr_default(void);
extern void isr32(void);
extern void isr33(void);

static struct idt_entry idt[256];

// array de ponteiros para funções C que tratam as IRQs
static void (*interrupt_handlers[256])(void) = {0};

void register_interrupt_handler(int n, void (*handler)(void)) {
    interrupt_handlers[n] = handler;
}

void irq_handler(int irq) {
    if (interrupt_handlers[irq]) {
        interrupt_handlers[irq]();
    }
    if (irq >= 40) {
    	outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

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
    pic_remap();
    for (int i = 0; i < 256; i++)
        idt_set_gate(i, (uint64_t)isr_default);

    idt_set_gate(33, (uint64_t)isr33);
    idt_set_gate(32, (uint64_t)isr32);

    struct idt_ptr idtr = {
        .limit = sizeof(idt) - 1,
        .base  = (uint64_t)&idt
    };

    asm volatile ("lidt %0" : : "m"(idtr));
    asm volatile ("sti");
}

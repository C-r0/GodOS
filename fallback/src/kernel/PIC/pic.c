#include "../include/pic.h"
#include "../include/io.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2+1)
#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

void pic_remap(void) {
    // 1. Iniciar Inicialização
    outb(PIC1_COMMAND, 0x11); 
    outb(PIC2_COMMAND, 0x11);

    // 2. Definir Offsets (ICW2) 
    outb(PIC1_DATA, 0x20); // IRQs 0-7 vão para 0x20-0x27
    outb(PIC2_DATA, 0x28); // IRQs 8-15 vão para 0x28-0x2F

    // 3. Configuração Master/Slave (ICW3)
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    // 4. Modo 8086 (ICW4)
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // 5. MÁSCARAS 
    outb(PIC1_DATA, 0xFC); // Habilita Teclado e System Timer
    outb(PIC2_DATA, 0xFF); // Desabilita tudo no Slave
}

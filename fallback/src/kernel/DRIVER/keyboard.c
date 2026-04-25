#include "../include/keyboard.h"
#include "../include/idt.h"
#include "../include/vga.h"
#include "../include/shell.h"
#include "../include/io.h"

static int shift_pressed = 0;

static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,   '\\','z','x','c','v','b','n','m',',','.','/',
    0,   '*', 0, ' '
};

static const char keymap_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,   'A','S','D','F','G','H','J','K','L',':','"','~',
    0,   '|','Z','X','C','V','B','N','M','<','>','?',
    0,   '*', 0, ' '
};

void keyboard_callback(void) {
    unsigned char scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }

    if (scancode & 0x80)
        return; 

    char c = shift_pressed ? keymap_shift[scancode] : keymap[scancode];
    if (c) {
    	shell_handle_input(c);
    }
}

void keyboard_init(void) {
    while (inb(0x64) & 1) { // Limpa buffer
    	inb(0x60);
    }

    outb(0x64, 0xAE);

    outb(0x64, 0x20); 
    uint8_t cb = inb(0x60);
    cb |= 0x01;       
    cb &= ~0x10;      
    outb(0x64, 0x60); 
    outb(0x60, cb);

    register_interrupt_handler(33, keyboard_callback); // IRQ1 = 32 + 1
}

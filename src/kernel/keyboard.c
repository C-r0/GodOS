#include "include/keyboard.h"
#include "include/idt.h"
#include "include/vga.h"
#include "include/shell.h"
#include "include/io.h"

static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,   '\\','z','x','c','v','b','n','m',',','.','/',
    0,   '*', 0, ' '
};

void keyboard_callback(void) {
    unsigned char scancode = inb(0x60);

    if (scancode & 0x80)
        return; // tecla solta

    char c = keymap[scancode];
    if (c)
        shell_handle_input(c);
}

void keyboard_init(void) {
    register_interrupt_handler(33, keyboard_callback); // IRQ1 = 32 + 1
}

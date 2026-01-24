#include "include/idt.h"
#include "include/vga.h"
#include "include/shell.h"
#include "include/string.h"
#include "include/keyboard.h"
#include "include/pic.h"

void kernel_main(void) {
	pic_remap();
    idt_init();
	
	vga_clear();
	vga_print("Welcome To GodOS\n");
	vga_print("Help for commands\n");
	
	shell_init();
	
	keyboard_init();

    for (;;);
}

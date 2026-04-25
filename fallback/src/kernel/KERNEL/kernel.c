#include "../include/idt.h"
#include "../include/vga.h"
#include "../include/shell.h"
#include "../include/string.h"
#include "../include/keyboard.h"
#include "../include/pic.h"
#include "../include/memory.h"
#include "../include/io.h"
#include "../include/timer.h"

void protect_kernel_in_bitmap() {
    uint64_t end_addr = (uintptr_t)&_kernel_end; 
    uint64_t end_page = (end_addr + PAGE_SIZE - 1) / PAGE_SIZE; 

    for (uint64_t p = 0; p < end_page; p++) {
        bitmap_set(p);
    }
}

void read_memory_map(void* mmap_ptr) {
    uint16_t entry_count = *(uint16_t*)mmap_ptr;
    struct e820_entry* mmap = (struct e820_entry*)((uintptr_t)mmap_ptr + 4);

    for(int i = 0; i < (MAX_PAGES / 64); i++) bitmap[i] = 0xFFFFFFFFFFFFFFFF;

    for (uint16_t i = 0; i < entry_count; i++) {
        if (mmap[i].type == 1) { 
            vga_putc('1', 0x02);

            uint64_t start_page = mmap[i].addr / PAGE_SIZE;
            uint64_t num_pages = mmap[i].len / PAGE_SIZE;

            for (uint64_t j = 0; j < num_pages; j++) {
                bitmap_free(start_page + j);
            }
        } else {
            vga_putc('2', 0x02);
        }
    }

    protect_kernel_in_bitmap();
}

void kernel_main(uint32_t magic, void* mbi_ptr) {
    vga_clear();

    if (magic != 0x2BADB002) {
        vga_print("Error: Bootloader not recognized!\n", 0x04);
        for(;;);
    }

    pic_remap();
    idt_init();

    // Passa o ponteiro 0x8000 para a função de leitura
    read_memory_map(mbi_ptr);

	set_mbi(mbi_ptr);
    
    vga_print("|Memory mapped successfully (E820)\n", 0x02);
    shell_show_logo();
    vga_print("Welcome To ", 0x0F); vga_print("GodOS\n", 0x0E);
    vga_print("Type ", 0x0F); vga_print("help ", 0x0B); vga_print("for commands\n", 0x0F);

    shell_startup();
    shell_init();
    keyboard_init();

    // Loop principal
    while(1) {
	asm volatile("hlt");
    }
}

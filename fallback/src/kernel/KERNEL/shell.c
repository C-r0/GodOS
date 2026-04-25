#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/string.h" 
#include "../include/memory.h"
#include "../include/math.h"
#include <stdint.h>

extern uint8_t g_current_color;

// COISINHA BUNITINHA UI UI UI UI
void shell_show_logo() {
    vga_print("\n", 0x0F);
    vga_print("  ________            ________  _________ \n", 0x0E);
    vga_print(" /  _____/  ____   __|  |  |  \\/   _____/ \n", 0x0E);
    vga_print("/   \\  ____/  _ \\ / __ |  |  /\\_____  \\  \n", 0x0E);
    vga_print("\\    \\_\\  (  <_> ) /_/ |  |  / /        \\ \n", 0x0E);
    vga_print(" \\______  /\\____/\\____ |____/ /_______  / \n", 0x0E);
    vga_print("        \\/            \\/              \\/  \n", 0x0E);
    vga_print("             The God Operating System\n", 0x0F);
    vga_print("------------------------------------------\n", 0x07);
}

void draw_ram_bar() {
    uint32_t usada_mb = g_total_bytes_usados >> 20;
    uint32_t total_mb = g_total_ram_mb;
    
    uint32_t num_hashes = (usada_mb * 64) / total_mb;

    vga_print("############################## System Information ##############################", g_current_color);

    vga_print("RAM [", 0x0F);

    for (uint32_t i = 0; i < num_hashes; i++) {
        vga_print("#", 0x0A); 
    }

    for (uint32_t i = num_hashes; i < 64; i++) {
        vga_print(".", 0x08); 
    }

    vga_print("] ", 0x0F);

    uint32_t porcentagem = (usada_mb * 100) / total_mb;
    char perc_buf[10];
    itoa(porcentagem, perc_buf);
    vga_print(perc_buf, 0x0B);
    vga_print("%", 0x0B);
}

// NAO SEI PRA QUE ESSA MERDA TA AQUI
int execute_command(const char *cmd);

void shell_init() {
    vga_print("root@GodOS:/$ ", 0x0F);
}

#define BUFFER_SIZE 128
volatile char buffer[BUFFER_SIZE] __attribute__((aligned(16)));
volatile int index __attribute__((aligned(16))) = 0;

// HANDLE INPUT

int shell_focused = 1;

// ME SALVOU PARA ESCREVER NO NOTEBOOK AEEEEEEE
void shell_startup() {
    index = 0; 
    for(int i=0; i<BUFFER_SIZE; i++) buffer[i] = 0;
    shell_focused = 1;
}

void shell_handle_input(char c) {
    if (shell_focused == 0) {
        if (c == 27) {
            shell_focused = 1;
            vga_clear();
            shell_init();
        }
        return;
    }

    if (c == '\n') {
        buffer[index] = 0;
        int needs_newline = execute_command(buffer);
        index = 0;
        if (shell_focused == 1) {
	    if (needs_newline) vga_putc('\n', 0x0F);
            shell_init();
        }
    } else if (c == '\b') {
        if (index > 0) {
            index--;
            vga_putc('\b', 0x0F);
        }
    } else {
        if (index < BUFFER_SIZE - 1) {
            buffer[index++] = c;
            vga_putc(c, g_current_color);
        }
    }
}

// COMANDOS
int execute_command(const char *cmd) {
    char* args[10];
    int arg_c = 0;

    parse_space((char*)cmd, args, &arg_c);
    
    if (arg_c == 0) return 1;
    // CLEAR
    if (strcmp(args[0], "clear") == 0) {
        vga_clear();
	return 0;
    // HELP
    } else if (strcmp(args[0], "help") == 0) {
        vga_print("\nclear", 0x03); vga_print(" - Clears the screen", 0x0F);
        vga_print("\nhelp", 0x03); vga_print(" - Shows this message", 0x0F);
        vga_print("\nmem", 0x03); vga_print(" - Shows memory entries", 0x0F);
        vga_print("\necho \"hello\"", 0x03); vga_print(" - Prints any message you want", 0x0F);
        vga_print("\ncolor 5", 0x03); vga_print(" - Set a color for what you write (0 - 15)", 0x0F);
        vga_print("\ncalc 10 + 10", 0x03); vga_print(" - A calculator (10 + 5, 10 x 5, 10 * 5, 10 / 5, 10 - 5)", 0x0F);
        vga_print("\ngodfetch", 0x03); vga_print(" - Shows the logo and system information", 0x0F);
        vga_print("\ntop", 0x03); vga_print(" - Monitors the system", 0x0F);
        vga_print("\nalloc 1", 0x03); vga_print(" - Allocate Memory (RAM)", 0x0F);
        vga_print("\nfree addr pages", 0x03); vga_print(" - Free Memory (RAM)", 0x0F);
    // MEM
    } else if (strcmp(args[0], "mem") == 0) {
        vga_print("\n--- BIOS Memory Map ---", 0x02);
        for (int i = 0; i < g_mmap_count; i++) {
            char buf[24];
            vga_print("\nEntry ", 0x02); itoa(i, buf); vga_print(buf, 0x02);
            vga_print(": Base=", 0x02); itoa(g_mmap[i].addr, buf); vga_print(buf, 0x02);
            vga_print(" Size=", 0x02); itoa(g_mmap[i].len, buf); vga_print(buf, 0x02);
            vga_print(" Type=", 0x02); itoa(g_mmap[i].type, buf); vga_print(buf, 0x02);
        }
        vga_print("\nUtilizable RAM: ", 0x02);
        vga_print(total_ram_buff, 0x02);
        vga_print(" MB | ", 0x02);
        vga_print("Total Kernel: ", 0x02); vga_print(kf_buffb, 0x02); vga_print(" B / ", 0x02); 
        vga_print(kf_buffkb, 0x02); vga_print(" KB / ", 0x02); vga_print(kf_buffmb, 0x02); vga_print(" MB", 0x02);
    // ECHO
    } else if (strncmp(args[0], "echo", 4) == 0){
        vga_print("\n", 0x0F);
        for (int i = 1; i < arg_c; i++) {
        vga_print(args[i], g_current_color);
        if (i < arg_c - 1) vga_print(" ", 0x0F); 
    }
    // COLOR
    } else if (strncmp(args[0], "color", 5) == 0){
        int cor = atoi(args[1]);
        if (cor >= 0 && cor <= 15) {
            vga_set_color((uint8_t) cor);
        } else {
           vga_print("\nError: only 0 - 15", 0x04);
        }
    // CALC
    } else if (strcmp(args[0], "calc") == 0) {
        if (arg_c < 4) {
		vga_print("\nUsage: calc 10 + 10 (+ - x / *)", 0x0E);
		return 0;
	}
        int n1 = atoi(args[1]);
        char op = args[2][0];
        int n2 = atoi(args[3]);
        int res = 0;
        if (op == '+') res = n1 + n2;
        else if (op == '-') res = n1 - n2;
        else if (op == 'x') res = n1 * n2;
        else if (op == '/') {if (n2 == 0) {vga_print("\nYou can't divide by 0 brochacho >:(", 0x04); return 0;} res = n1 / n2;}
        else if (op == '*') res = power(n1, n2);
	else {vga_print("\nUse a valid operator (+ - x / *)", 0x0E); return 0;}
        char buff[32];
        itoa(res, buff);
        vga_print("\n", 0x0F);
        vga_print(buff, g_current_color);
    // GODFETCH
    } else if (strcmp(args[0], "godfetch") == 0){
        shell_show_logo();
        vga_print("RAM ", 0x02); vga_print(kf_buffkb, 0x02); vga_print(" KB / ", 0x02); vga_print(total_ram_buff, 0x02); vga_print(" MB \n", 0x02);
    // TOP (MUITO TOP TOP TOP TOP)
    } else if (strcmp(args[0], "top") == 0) {
        shell_focused = 0;
        vga_clear();
        draw_ram_bar();
        vga_print("\nRAM ", 0x02); vga_print(kf_buffkb, 0x02); vga_print(" KB / ", 0x02); vga_print(total_ram_buff, 0x02); vga_print(" MB \n", 0x02);
    // ALLOC
    } else if (strncmp(args[0], "alloc", 5) == 0) {
    int quantidade = atoi(args[1]);

    if (arg_c <= 0) {
        vga_print("\nError: you cant put negative numbers", 0x04);
        return 0;
    } else if (arg_c > 2) {
        vga_print("\nError: Only one argument (number of pages)", 0x04);
        return 0;
    }

    void* ptr = pmm_alloc_pages(quantidade); 

    if (ptr != 0) {
        char buf[32];
        h_itoa((uintptr_t)ptr, buf);

        vga_print("\nBlock of ", 0x0F);
        char q_buf[10]; itoa(quantidade, q_buf); vga_print(q_buf, 0x0B);
        vga_print(" pages alocated in: 0x", 0x0F);
        vga_print(buf, 0x0A); 
        
        vga_print("\n(End of block in: 0x", 0x07);
        h_itoa((uintptr_t)ptr + (quantidade << 12), buf);
        vga_print(buf, 0x07); vga_print(")", 0x07);
    } else {
        vga_print("\nError: Memory not found", 0x04);
    }
    // FREE
    } else if (strncmp(args[0], "free", 4) == 0) {
        if (arg_c < 3) {
            vga_print("\nUse: free address pages", 0x04);
            return 0;
        }
        
        uint32_t addr = hex_to_int(args[1]);
        uint32_t pages = atoi(args[2]);

        if (addr == 0) {
            vga_print("Invallid addr", 0x04);
        }

        pmm_free_pages((void*)(uintptr_t)addr, pages);

    // COMMAND NOT FOUND
    } else {
        vga_print("\nNo Command Found", 0x04);
    }
    return 1;
}

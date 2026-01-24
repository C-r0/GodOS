#include "include/shell.h"
#include "include/vga.h"
#include "include/string.h"

static void execute_command(const char *cmd);

void shell_init() {
	vga_print("\n> ");
}

#define BUFFER_SIZE 128
static char buffer[BUFFER_SIZE];
static int index = 0;

void shell_handle_input(char c) {
    if (c == '\n') {       // Tecla Enter
        buffer[index] = 0; // Termina string
        execute_command(buffer);
        index = 0;         // Reseta buffer
        vga_print("\n> "); // Prompt
    } else if (c == '\b') { // Backspace
        if (index > 0) {
            index--;
            vga_putc('\b'); // Remove caractere da tela
        }
    } else {
        if (index < BUFFER_SIZE - 1) {
            buffer[index++] = c;
            vga_putc(c);   // Mostra caractere
        }
    }
}

/* IMPLEMENTAÇÃO */
static void execute_command(const char *cmd) {
    if (strcmp(cmd, "clear") == 0) {
        vga_clear();
    }
    if(strcmp(cmd, "help")== 0) {
		vga_print("\nclear - Clears the screen");
		vga_print("\nhelp - Shows this message");
    } else {
        vga_print("\nNo Command Found");
    }
}


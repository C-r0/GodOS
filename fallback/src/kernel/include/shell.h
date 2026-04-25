#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

void shell_init();
void shell_startup();
void shell_handle_input(char c);
int execute_command(const char *cmd);
void shell_set_mbi(void* ptr); 
void shell_show_logo();
void shell_read_memory();
void refresh_memory_strings();

extern uint64_t g_total_ram_mb;
extern uint64_t g_total_bytes_usados;

extern volatile char buffer[128];
extern volatile int index;

#endif

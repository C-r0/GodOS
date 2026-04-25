#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define MAX_PAGES 131072 

struct e820_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t extended;
} __attribute__((packed));

extern uint64_t bitmap[MAX_PAGES / 64];
extern uint64_t g_total_ram_mb;
extern uint64_t g_total_bytes_usados;
extern uint8_t _kernel_end; 
extern uint16_t g_mmap_count;
extern struct e820_entry* g_mmap;
extern char total_ram_buff[32];
extern char kf_buffb[32];
extern char kf_buffkb[32];
extern char kf_buffmb[32];

void bitmap_set(uint32_t page_index);
void bitmap_free(uint32_t page_index);
int bitmap_test(uint32_t page_index);
void* pmm_alloc_pages(int m);
void pmm_free_pages(void* addr, uint32_t num_pages);
void refresh_memory_strings();
void set_mbi(void* ptr);

#endif
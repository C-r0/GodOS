#include "../include/memory.h"
#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/string.h"

uint64_t bitmap[MAX_PAGES / 64];

static void* internal_mbi_ptr = 0;

// VAGABUNDAGEM PRA MAIS FPS OTIMIZAR RAM
uint64_t g_total_ram_mb = 0;
char total_ram_buff[32];
uint16_t g_mmap_count = 0;
struct e820_entry* g_mmap = 0;
uint64_t kf_u64 = 0;
uint64_t kernel_start = 0x1000;
char kf_buffb[32];
char kf_buffkb[32];
char kf_buffmb[32];
uint64_t g_total_bytes_usados = 0;

void refresh_memory_strings() {
    uint64_t used_kb = g_total_bytes_usados >> 10;
    uint64_t used_mb = g_total_bytes_usados >> 20;

    itoa(g_total_bytes_usados, kf_buffb);
    itoa(used_kb, kf_buffkb);
    itoa(used_mb, kf_buffmb);
}

void set_mbi(void* ptr) {
    internal_mbi_ptr = ptr;

    g_mmap_count = *(uint16_t*)internal_mbi_ptr;
    g_mmap = (struct e820_entry*)((uintptr_t)internal_mbi_ptr + 4);

    uint64_t total_bytes = 0;

    for (int i = 0; i < g_mmap_count; i++) {
        if (g_mmap[i].type == 1) {
            total_bytes += g_mmap[i].len;
        }
    }
    g_total_ram_mb = total_bytes >> 20;
    itoa(g_total_ram_mb, total_ram_buff);

    kf_u64 = (uint64_t)&_kernel_end - kernel_start;
    g_total_bytes_usados += kf_u64;
    refresh_memory_strings();


    char buf[20];
    itoa((uintptr_t)ptr, buf);
    vga_print("\nMBI recebido em: ", 0x0F);
    vga_print(buf, 0x0F);
}

void bitmap_set(uint32_t page_index) {
    bitmap[page_index / 64] |= (1ULL << (page_index % 64));
}

void bitmap_free(uint32_t page_index) {
    bitmap[page_index / 64] &= ~(1ULL << (page_index % 64));
}

int bitmap_test(uint32_t page_index) {
    return (bitmap[page_index / 64] & (1ULL << (page_index % 64))) != 0;
}

void* pmm_alloc_pages(int m) {
    uint32_t pages_found = 0;
    uint32_t start_page = 0;

    for (uint32_t i = 256; i < MAX_PAGES; i++) {
        if (!bitmap_test(i)) {
            if (pages_found == 0) start_page = i;
            pages_found++;

            if (pages_found == m) {
                for (uint32_t j = start_page; j < start_page + m; j++) {
                    bitmap_set(j);
                }

                g_total_bytes_usados += (uint64_t)m << 12;
                refresh_memory_strings();

                return (void*)(uintptr_t)((uint32_t)start_page << 12);
            }
        } else {
            pages_found = 0;
        }
    }
    return 0; 
}

void pmm_free_pages(void* addr, uint32_t num_pages) {
    uint32_t address_val = (uint32_t)(uintptr_t)addr;

    if (address_val < 0x100000) {
        vga_print("\nSecurity error: you can't free pages of kernel", 0x04);
        return;
    }

    if (address_val & 0xFFF) { 
        vga_print("\nError: disaligned addr, use only x4096 (0x1000).", 0x0C);
        return;
    }
    
    uint32_t start_page = address_val >> 12;

    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t page = start_page + i;

        if (page >= MAX_PAGES) break;

        bitmap_free(page);
    }

    g_total_bytes_usados -= (uint64_t)num_pages << 12;

    refresh_memory_strings();
    vga_print("\nMemory free sucessfully", 0x0A);
}

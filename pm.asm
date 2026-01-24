[BITS 32]

global start
extern kernel_main

PML4_ADDR equ 0x20000
PDPT_ADDR equ 0x21000
PD_ADDR   equ 0x22000

start:
    cli
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x9000

    mov word [0xB8000], 0x0F50 ; P

    call setup_paging
    lgdt [gdt64_descriptor]

    mov eax, PML4_ADDR
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp 0x08:long_mode_entry


setup_paging:
    mov edi, PML4_ADDR
    mov ecx, 4096*3/4
    xor eax, eax
    rep stosd

    mov dword [PML4_ADDR], PDPT_ADDR | 3
    mov dword [PDPT_ADDR], PD_ADDR | 3
    mov dword [PD_ADDR], 0x83
    ret


ALIGN 8
gdt64:
    dq 0
    dq 0x00AF9A000000FFFF
    dq 0x00AF92000000FFFF
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64 - 1
    dd gdt64


[BITS 64]
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x90000

    call kernel_main

halt:
    hlt
    jmp halt

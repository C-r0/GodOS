[bits 16]
[org 0x7C00]

start:
    mov [BOOT_DRIVE], dl
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; A20
    in al, 0x92
    or al, 2
    out 0x92, al

    ; load stage2 (25 setores) em 0x1000
    xor ax, ax
    mov es, ax
    mov bx, 0x1000

    mov ah, 0x02
    mov al, 25
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc $

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:0x1000

BOOT_DRIVE db 0

gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xAA55

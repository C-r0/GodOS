[bits 16]
[org 0x7C00]

jmp short start
nop

times 33 db 0 ; BPB


start:
    mov [BOOT_DRIVE], dl

    xor ax, ax
    int 0x13

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

    ; COLETAR MAPA DE MEMÓRIA (E820)
    mov di, 0x0504    
    xor ebx, ebx
    xor bp, bp        
.mmap_loop:
    mov edx, 0x534D4150
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    jc .mmap_done
    
    inc bp              
    add di, 24          
    
    test ebx, ebx       
    jz .mmap_done
    
    cmp bp, 20          
    jne .mmap_loop
.mmap_done:
    mov [0x0500], bp    
    mov word [0x0502], 0

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

[bits 16]
[org 0x7c00]

    mov [BOOT_DRIVE], dl ; Salva o drive de boot logo no início

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; --- RESET DISK ---
    xor ax, ax          ; ah = 0 (reset disk), al = 0
    mov dl, [BOOT_DRIVE]
    int 0x13

    ; --- LOAD PM.BIN ---
    mov ah, 0x02        ; READ SECTOR
    mov al, 1           ; 32 SECTOR (pm.bin)
    mov ch, 0           
    mov cl, 2           ; SECTOR 2 
    mov dh, 0           ; HEAD 0
    mov dl, [BOOT_DRIVE]
    mov bx, 0x1000      
    int 0x13
    jc disk_error       


    mov ah, 0x0e
    mov al, 'L'
    int 0x10

    ; PROTECTED MODE
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:0x1000

disk_error:
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
    jmp $

BOOT_DRIVE db 0 

; --- GDT AND SING ---
gdt_start: dq 0x0
gdt_code:  dw 0xFFFF, 0x0000, 0x9A00, 0x00CF 
gdt_data:  dw 0xFFFF, 0x0000, 0x9200, 0x00CF
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xaa55

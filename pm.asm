[bits 32]

pmode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; P
    mov [0xb8000], word 0x0F50 
    
    jmp $

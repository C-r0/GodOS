[BITS 64]

global isr33
extern keyboard_callback
extern outb

isr33:
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi

    call keyboard_callback

    ; Enviar EOI ao PIC
    mov al, 0x20
    mov dx, 0x20   ; PIC1 command port
    out dx, al

    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    iretq

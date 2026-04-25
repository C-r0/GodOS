global inb
global outb

inb:
    mov dx, di
    in al, dx
    ret

outb:
    mov dx, di
    mov al, sil
    out dx, al
    ret

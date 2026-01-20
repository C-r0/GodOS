[bits 16]
[org 0x7C00]

start:
	mov ah, 0x0E ; BIOS teletype function number
    mov al, 'B'  ; The character to print is placed in the AL register
    int 0x10     ; Call the video interrupt to print the character

    jmp $        ; Infinite loop to stop the program (for boot sector use)

    times 510 - ($-$$) db 0 ; Fill the rest of the sector with zeros
    dw 0xAA55    ; Boot sector signature

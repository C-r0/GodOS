[BITS 64]
global isr_default

isr_default:
    cli
.hang:
    hlt
    jmp .hang

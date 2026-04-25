ENTRY(start)

SECTIONS
{
  . = 0x1000;

  .text : { *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(COMMON) *(.bss*) }

  _kernel_end = .;
}

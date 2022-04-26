OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x10000;
    .text :
    {
        *(.text);
        *(.rodata);
    }
    .data :
    {
        *(.data);
        *(.bss);
        *(.rodata);
    }
    _heap = ALIGN(4);
}

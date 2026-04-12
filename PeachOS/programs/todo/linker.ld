/* Program entry symbol provided by userspace startup code. */
ENTRY(_start)
/* Emit a 32-bit i386 ELF image expected by PeachOS loader. */
OUTPUT_FORMAT(elf32-i386)
SECTIONS
{
    /* Load user program at conventional userspace base address. */
    . = 0x400000;
    /* Executable code section. */
    .text : ALIGN(4096)
    {
        *(.text)
    }

    /* Assembly helper section used by low-level startup/runtime code. */
    .asm : ALIGN(4096)
    {
        *(.asm)
    }

    /* Read-only constants and string literals. */
    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    /* Initialized global/static data. */
    .data : ALIGN(4096)
    {
        *(.data)
    }

    /* Zero-initialized globals plus compiler common symbols. */
    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }
}

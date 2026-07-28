/* Memory Map for the QEMU MPS2-AN500 (Cortex-M7) Board */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 2M   /* ROM / Code */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 2M   /* SRAM / Variables */
}

/* Define the top of the stack at the very end of RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* The entry point of our bare-metal system */
ENTRY(Reset_Handler)

SECTIONS
{
    /* 1. The Vector Table and Code go into FLASH */
    .text :
    {
        KEEP(*(.isr_vector)) /* Vector table MUST be absolutely first at 0x0 */
        *(.text*)            /* All program code */
        *(.rodata*)          /* Read-only data (constants, strings) */
        . = ALIGN(4);
        _etext = .;          /* Save the end address of the code */
    } > FLASH

    /* 2. Initialized variables go into RAM (but are stored in FLASH initially) */
    .data : AT(_etext)
    {
        . = ALIGN(4);
        _sdata = .;          /* Start address of data in RAM */
        *(.data*)            /* Initialized variables */
        . = ALIGN(4);
        _edata = .;          /* End address of data in RAM */
    } > RAM

    /* 3. Uninitialized variables go into RAM */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;           /* Start address of bss */
        *(.bss*)             /* Uninitialized variables (zeroed out on boot) */
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;           /* End address of bss */
    } > RAM
}
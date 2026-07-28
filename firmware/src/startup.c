#include <stdint.h>

/* These variables are defined in our linker.ld script */
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

/* Function prototype for main */
extern int main(void);

/* Default interrupt handler for unexpected faults */
void Default_Handler(void) {
    while (1) {
        // Infinite loop to trap the processor if a fault occurs
    }
}

/* The first function that executes when the MCU gets power */
void Reset_Handler(void) {
    uint32_t *src, *dest;

    /* 1. Copy the initialized data (.data) from Flash to RAM */
    src = &_etext;
    for (dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }

    /* 2. Zero out the uninitialized data (.bss) in RAM */
    for (dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }

    /* 3. Call the main() function */
    main();

    /* 4. If main() somehow exits, trap the processor */
    while (1);
}

/* The Interrupt Vector Table (IVT) */
/* The linker script ensures this array is placed exactly at 0x00000000 */
__attribute__((section(".isr_vector"), used))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))(&_estack), /* 0: Initial Stack Pointer */
    Reset_Handler,              /* 1: Reset Handler (Boot execution point) */
    Default_Handler,            /* 2: NMI Handler */
    Default_Handler,            /* 3: Hard Fault Handler */
    Default_Handler,            /* 4: Memory Management Handler */
    Default_Handler,            /* 5: Bus Fault Handler */
    Default_Handler,            /* 6: Usage Fault Handler */
    0, 0, 0, 0,                 /* 7-10: Reserved */
    Default_Handler,            /* 11: SVCall Handler */
    Default_Handler,            /* 12: Debug Monitor Handler */
    0,                          /* 13: Reserved */
    Default_Handler,            /* 14: PendSV Handler */
    Default_Handler             /* 15: SysTick Handler */
};
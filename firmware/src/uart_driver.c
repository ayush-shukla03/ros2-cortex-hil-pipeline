#include "uart_driver.h"
#include <stdint.h>

// CMSDK APB UART Register Map for MPS2
volatile uint32_t * const UART0_DATA   = (uint32_t *)0x40004000;
volatile uint32_t * const UART0_STATE  = (uint32_t *)0x40004004;
volatile uint32_t * const UART0_CTRL   = (uint32_t *)0x40004008;

#define UART_TX_ENABLE (1 << 0)
#define UART_RX_ENABLE (1 << 1)
#define UART_RX_FULL   (1 << 1) // Bit 1 in state register

void uart_init() {
    // Enable both Transmit and Receive hardware circuits
    *UART0_CTRL = UART_TX_ENABLE | UART_RX_ENABLE; 
}

void uart_send_char(char c) {
    *UART0_DATA = (uint32_t)c;
}

void uart_send_string(const char *s) {
    while(*s != '\0') {
        uart_send_char(*s);
        s++;
    }
}

// Blocking read: Waits until the RX hardware buffer has data
uint8_t uart_read_byte() {
    while ((*UART0_STATE & UART_RX_FULL) == 0) {
        // Spin and wait for incoming data
    }
    return (uint8_t)(*UART0_DATA);
}
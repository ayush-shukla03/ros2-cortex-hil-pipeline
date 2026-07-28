#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>

// Initializes the UART peripheral by enabling TX and RX hardware
void uart_init(void);

// Transmits a single character over the UART interface
void uart_send_char(char c);

// Transmits a null-terminated string over the UART interface
void uart_send_string(const char *s);

// Blocks execution until a byte is received, then returns it
uint8_t uart_read_byte(void);

#endif // UART_DRIVER_H
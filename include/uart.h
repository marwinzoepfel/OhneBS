// include/uart.h
#ifndef UART_H
#define UART_H

#include "string_utils.h" // Für die 'bool' Definition

void uart_init();
void uart_writeText(const char *buffer);
void uart_writeByteBlocking(unsigned char ch); // Nützliche Hilfsfunktion
bool uart_read_byte(unsigned char* byte);

#endif // UART_H

// src/kernel.c
#include "uart.h"
#include "shell.h"

void kernel_main() {
    uart_init();
    shell_init();

    uart_writeText("Welcome to OhneBS!\n");

    while (1) {
        shell_update(); // Die richtige Update-Funktion aufrufen
    }
}

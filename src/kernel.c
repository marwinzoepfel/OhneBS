// src/kernel.c
#include "uart.h"
#include "shell.h"
#include "fb.h"

void kernel_main() {
    uart_init();
    shell_init();
    fb_init();

    uart_writeText("Welcome to OhneBS!\n");
    drawRect(150,150,400,400,0x03,0);
    drawRect(300,300,350,350,0x2e,1);

    drawCircle(960,540,250,0x0e,0);
    drawCircle(960,540,50,0x13,1);

    drawPixel(250,250,0x0e);

    drawChar('O',500,500,0x05);
    drawString(100,100,"Hello world!",0x0f);

    drawLine(100,500,350,700,0x0c);
    
    while (1) {
        shell_update(); // Die richtige Update-Funktion aufrufen

    }
}

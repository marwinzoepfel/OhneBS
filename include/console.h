// include/console.h
#ifndef CONSOLE_H
#define CONSOLE_H



// Initialisiert UART und Framebuffer für die Konsole
void console_init();

// Schreibt ein einzelnes Zeichen auf die Konsole (UART und Framebuffer)
void console_putc(char c);

// Schreibt einen String auf die Konsole (UART und Framebuffer)
void console_puts(const char* s);

// Schreibt eine vorzeichenbehaftete Ganzzahl auf die Konsole
void console_putint(int i);

// Schreibt eine vorzeichenlose Ganzzahl im Hexadezimalformat auf die Konsole
void console_puthex(unsigned int val);

#endif // CONSOLE_H

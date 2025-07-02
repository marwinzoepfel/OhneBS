// src/console.c
#include "console.h"
#include "uart.h"
#include "fb.h"
#include "string_utils.h" // Für simple_itoa, simple_uint_to_hex_string

// Konfiguration für die Textdarstellung auf dem Framebuffer
#define FONT_WIDTH 8      // Annahme: Breite eines Zeichens in Pixeln
#define FONT_HEIGHT 16    // Annahme: Höhe eines Zeichens in Pixeln
#define TEXT_COLOR 0x0F   // Weiß (Annahme: 0x0F ist Weiß in deiner Farbpalette)
#define BG_COLOR 0x00     // Schwarz (Annahme: 0x00 ist Schwarz)

// Aktuelle Cursorposition auf dem Framebuffer
static unsigned int current_x = 0;
static unsigned int current_y = 0;

// Bildschirmdimensionen (müssen von fb.h oder fb_init abgerufen werden)
// Annahme: Deine fb_init setzt diese oder es gibt globale Variablen/Funktionen dafür.
// Fürs Beispiel feste Werte, idealerweise aus fb.h/fb_get_resolution()
#define SCREEN_WIDTH 1920 // Beispiel Raspberry Pi typische Auflösung
#define SCREEN_HEIGHT 1080 // Beispiel Raspberry Pi typische Auflösung

// Hilfsfunktion für den Zeilenumbruch und Scrollen
static void console_newline() {
    current_x = 0;
    current_y += FONT_HEIGHT;

    // Wenn der untere Bildschirmrand erreicht ist
    if (current_y + FONT_HEIGHT > SCREEN_HEIGHT) {
        // --- Einfaches Scrollen (Bildschirm löschen und von oben anfangen) ---
        // Das ist die einfachste Methode, aber nicht "flüssig".
        // drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR, 1); // Alles löschen
        // current_y = 0;

        // --- Besser: Tatsächliches Scrollen durch Verschieben des Bildinhalts ---
        // Dies erfordert, dass du eine Funktion hast, die einen Bereich des FB kopiert.
        // Wenn du keine hast, musst du sie in fb.c hinzufügen oder die oben einfache Methode nehmen.
        // Beispiel: scroll_framebuffer(FONT_HEIGHT, BG_COLOR); // Verschiebt alles um FONT_HEIGHT Pixel nach oben
        // current_y -= FONT_HEIGHT; // Bleibt auf der letzten sichtbaren Zeile, da der Inhalt gescrollt wurde.

        // Hier ist eine rudimentäre Scroll-Logik, die Pixel für Pixel verschiebt.
        // Besser wäre eine optimierte Funktion in fb.c, die größere Blöcke verschiebt.
        // Für diesen Prototypen nehmen wir die einfache "clear and reset" Methode,
        // da eine performante Scroll-Implementierung tiefer in fb.c gehen würde.
        // Wenn du eine drawImage/copyRect-Funktion hast, wäre es:
        // copy_rect(0, FONT_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT);
        // drawRect(0, SCREEN_HEIGHT - FONT_HEIGHT, SCREEN_WIDTH, FONT_HEIGHT, BG_COLOR, 1); // Letzte Zeile leeren
        // current_y = SCREEN_HEIGHT - FONT_HEIGHT; // Bleibt auf der letzten Zeile


        // Prototypen-Lösung (löscht und setzt Cursor zurück):
        drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR, 1); // Löscht den gesamten Bildschirm
        current_x = 0;
        current_y = 0;
    }
}


void console_init() {
    uart_init(); // UART initialisieren
    fb_init();   // Framebuffer initialisieren

    // Bildschirm löschen und Cursorposition initialisieren
    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR, 1); // Hintergrundfarbe setzen
    current_x = 0;
    current_y = 0;
}

void console_putc(char c) {
    uart_writeByteBlocking(c); // Immer auf UART schreiben

    // Spezialbehandlung für Steuerzeichen für den Framebuffer
    if (c == '\n') {
        uart_writeByteBlocking('\r');
        console_newline(); // Zeilenumbruch und Scrollen
    } else if (c == '\r') {
        current_x = 0; // Cursor an den Anfang der aktuellen Zeile
    } else if (c == '\b') {
        // Backspace: Cursor zurück und Zeichen löschen
        if (current_x >= FONT_WIDTH) {
            current_x -= FONT_WIDTH;
            drawRect(current_x, current_y, FONT_WIDTH, FONT_HEIGHT, BG_COLOR, 1); // Zeichen löschen
        }
    } else {
        // Normales druckbares Zeichen
        drawChar(c, current_x, current_y, TEXT_COLOR);
        current_x += FONT_WIDTH;

        // Zeilenumbruch, falls der rechte Rand erreicht ist
        if (current_x + FONT_WIDTH > SCREEN_WIDTH) {
            console_newline();
        }
    }
}

void console_puts(const char* s) {
    while (*s != '\0') {
        console_putc(*s);
        s++;
    }
}

void console_putint(int i) {
    char buffer[12]; // Genug Platz für -2,147,483,648 und Null-Terminator
    console_puts(simple_itoa(i, buffer));
}

void console_puthex(unsigned int val) {
    char buffer[10]; // Genug Platz für 0xFFFFFFFF und Null-Terminator
    console_puts("0x");
    console_puts(simple_uint_to_hex_string(val, buffer)); // Annahme: simple_uint_to_hex_string existiert
}

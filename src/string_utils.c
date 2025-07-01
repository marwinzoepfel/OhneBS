#include "string_utils.h"

// ##################################
// ## Private Hilfsfunktionen
// ##################################

/**
 * Kehrt einen String um. 'static', da nur intern von simple_itoa gebraucht.
 */
static void reverse_string(char *str, int length) {
    int start = 0;
    int end = length - 1;
    char temp;
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// ##################################
// ## Öffentliche Funktionen
// ##################################

/**
 * Wandelt einen String in einen int um. Berücksichtigt negative Zahlen.
 */
int simple_atoi(const char *str) {
    int res = 0;
    bool is_negative = false;
    if (*str == '-') {
        is_negative = true;
        str++;
    }
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return is_negative ? -res : res;
}

/**
 * Wandelt einen int in einen String um.
 */
char* simple_itoa(int value, char *buffer) {
    int i = 0;
    bool is_negative = false;

    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }
    
    // Spezialfall für den minimalen int-Wert
    if (value == -2147483648) {
        // strncpy_simple ist hierfür nicht ideal, wir schreiben es manuell
        const char* min_int_str = "-2147483648";
        while(*min_int_str) {
            *buffer++ = *min_int_str++;
        }
        *buffer = '\0';
        return buffer - 11; // Zeiger auf den Anfang zurückgeben
    }

    if (value < 0) {
        is_negative = true;
        value = -value;
    }

    while (value != 0) {
        int rem = value % 10;
        buffer[i++] = rem + '0';
        value = value / 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';
    reverse_string(buffer, i);
    return buffer;
}

/**
 * Vergleicht zwei Strings.
 */
int strcmp_simple(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/**
 * Vergleicht die ersten 'n' Zeichen zweier Strings.
 */
int strncmp_simple(const char *s1, const char *s2, unsigned int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/**
 * Kopiert n Zeichen oder bis zum ersten Leerzeichen/Null-Terminator.
 */
void strncpy_simple(char *dest, const char *src, unsigned int n) {
    while (n > 1 && *src != '\0' && *src != ' ') {
        *dest++ = *src++;
        n--;
    }
    *dest = '\0';
}

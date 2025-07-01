// include/string_utils.h

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// =======================================================
// ## Zentrale Definitionen für das ganze Projekt ##
// =======================================================
#define true 1
#define false 0
typedef int bool;
#define NULL ((void*)0)  // <-- DIESE ZEILE HINZUFÜGEN
// =======================================================


// Öffentliche Funktionsprototypen
int simple_atoi(const char *str);
char* simple_itoa(int value, char *buffer);
int strcmp_simple(const char *s1, const char *s2);
int strncmp_simple(const char *s1, const char *s2, unsigned int n);
void strncpy_simple(char *dest, const char *src, unsigned int n);

#endif // STRING_UTILS_H

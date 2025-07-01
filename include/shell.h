// include/shell.h
#ifndef SHELL_H
#define SHELL_H

/**
 * Initialisiert die Shell. (Gute Praxis für später)
 */
void shell_init();

/**
 * Verarbeitet anstehende Benutzereingaben und führt Befehle aus.
 * Diese Funktion sollte in der Hauptschleife des Kernels aufgerufen werden.
 */
void shell_update();

#endif // SHELL_H

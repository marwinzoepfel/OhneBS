// src/shell.c

// ##################################
// ## Includes
// ##################################
// Die uart.h ist hier nicht mehr direkt für die Ausgabe nötig,
// da die console.h diese Funktionalität umschließt.
// Wir brauchen aber uart.h weiterhin für uart_read_byte!
#include "uart.h"          // Behalten, da wir von UART lesen
#include "shell.h"
#include "string_utils.h"
#include "console.h"       // NEU: console.h für die vereinheitlichte Ausgabe

// ##################################
// ## Private Datenstrukturen und globale Variablen
// ##################################

#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static unsigned int input_buffer_pos = 0;

#define MAX_VARIABLES 50
#define MAX_VAR_NAME_LENGTH 16

typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    int value;
} NamedVariable;

static NamedVariable variable_storage[MAX_VARIABLES];
static unsigned int variable_count = 0;

// ##################################
// ## Private Funktionsprototypen (nur für diese Datei sichtbar)
// ##################################
static void process_command(char *buffer);
static NamedVariable* find_variable(const char* name);
static void set_variable(const char* name, int value);
static int get_variable(const char* name, bool* success);
static int evaluate_term(const char* term, bool* success);
static int evaluate_expression(const char* expr, bool* success);


// ##################################
// ## Implementierung der öffentlichen Funktionen (aus shell.h)
// ##################################

void shell_init() {
    input_buffer_pos = 0;
    // Der Prompt wird jetzt hier initialisiert und in shell_update aufgerufen
    // (Oder direkt nach console_init() in kernel_main, wie vorgeschlagen)
}

void shell_update() {
    unsigned char byte;
    // Versuche, ein Byte von der UART zu lesen
    if (uart_read_byte(&byte)) { // WICHTIG: Hier weiterhin uart_read_byte() nutzen, da es der INPUT ist
        // Enter wurde gedrückt
        if (byte == '\r') {
            console_puts("\n"); // Ausgabe über die Konsole (geht an UART und FB)
            input_buffer[input_buffer_pos] = '\0';

            if (input_buffer_pos > 0) {
                process_command(input_buffer);
            }
            input_buffer_pos = 0;
            console_puts("> "); // Ausgabe über die Konsole
        }
        // Backspace
        else if ((byte == 0x08 || byte == 0x7F) && input_buffer_pos > 0) {
            input_buffer_pos--;
            console_puts("\b \b"); // Ausgabe über die Konsole (Backspace, Leerzeichen, Backspace)
        }
        // Normales, druckbares Zeichen
        else if (byte >= ' ' && byte <= '~' && input_buffer_pos < (INPUT_BUFFER_SIZE - 1)) {
            input_buffer[input_buffer_pos++] = byte;
            console_putc(byte); // Echo des Zeichens über die Konsole
        }
    }
}

// ##################################
// ## Implementierung der privaten Funktionen
// ##################################

// --- Variablenverwaltung ---
static NamedVariable* find_variable(const char* name) {
    for (unsigned int i = 0; i < variable_count; i++) {
        if (strcmp_simple(name, variable_storage[i].name) == 0) {
            return &variable_storage[i];
        }
    }
    return NULL;
}

static void set_variable(const char* name, int value) {
    NamedVariable* var = find_variable(name);
    if (var != NULL) {
        var->value = value;
    } else {
        if (variable_count < MAX_VARIABLES) {
            strncpy_simple(variable_storage[variable_count].name, name, MAX_VAR_NAME_LENGTH);
            variable_storage[variable_count].value = value;
            variable_count++;
        } else {
            console_puts("Error: Maximum number of variables reached!\n"); // Ausgabe über die Konsole
        }
    }
}

static int get_variable(const char* name, bool* success) {
    NamedVariable* var = find_variable(name);
    if (var != NULL) {
        if (success) *success = true;
        return var->value;
    }
    if (success) *success = false;
    return 0;
}

// --- Expression Evaluator ---
static int evaluate_term(const char* term, bool* success) {
    while (*term == ' ') term++;
    if ((*term >= '0' && *term <= '9') || *term == '-') {
        *success = true;
        return simple_atoi(term);
    } else {
        return get_variable(term, success);
    }
}

static int evaluate_expression(const char* expr, bool* success) {
    char lhs_str[MAX_VAR_NAME_LENGTH];
    char rhs_str[MAX_VAR_NAME_LENGTH];
    const char* plus_operator = NULL;

    for (const char* p = expr; *p != '\0'; p++) {
        if (*p == '+') {
            plus_operator = p;
            break;
        }
    }

    if (plus_operator == NULL) {
        return evaluate_term(expr, success);
    }

    int lhs_len = plus_operator - expr;
    strncpy_simple(lhs_str, expr, lhs_len + 1);
    strncpy_simple(rhs_str, plus_operator + 1, MAX_VAR_NAME_LENGTH);

    bool lhs_success, rhs_success;
    int lhs_value = evaluate_term(lhs_str, &lhs_success);
    int rhs_value = evaluate_term(rhs_str, &rhs_success);

    if (lhs_success && rhs_success) {
        *success = true;
        return lhs_value + rhs_value;
    }

    *success = false;
    return 0;
}


// ##################################
// ## Implementierung der privaten Funktionen (früher öffentlich aus shell.h)
// ##################################

static void process_command(char *buffer) {
    char command[10];
    char var_name[MAX_VAR_NAME_LENGTH];
    // char number_buffer[12]; // Nicht mehr direkt benötigt, da console_putint() das übernimmt

    int i = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0') {
        if (i < 9) { command[i] = buffer[i]; }
        i++;
    }
    command[i] = '\0';

    if (strcmp_simple(command, "help") == 0) {
        console_puts("Commands:\n - set <name> <value>\n - print <expr>\n - version\n"); // Ausgabe über die Konsole
    } else if (strcmp_simple(command, "version") == 0) {
        console_puts("OhneBS v0.1.0-alpha\n"); // Ausgabe über die Konsole
    } else if (strcmp_simple(command, "set") == 0) {
        char* name_start = buffer + i + 1;
        int j = 0;
        while(name_start[j] != ' ' && name_start[j] != '\0') j++;
        strncpy_simple(var_name, name_start, j + 1);
        char* value_start = name_start + j + 1;
        int value = simple_atoi(value_start);
        set_variable(var_name, value);
        console_puts("OK.\n"); // Ausgabe über die Konsole
    } else if (strcmp_simple(command, "print") == 0) {
        char* expression = buffer + i + 1;
        bool success;
        int result = evaluate_expression(expression, &success);
        if (success) {
            console_putint(result); // NEU: Nutzt console_putint()
            console_puts("\n");     // Ausgabe über die Konsole
        } else {
            console_puts("Error: Invalid expression or variable not found.\n"); // Ausgabe über die Konsole
        }
    } else {
        console_puts("Unknown command: '"); // Ausgabe über die Konsole
        console_puts(buffer);               // Ausgabe über die Konsole
        console_puts("'\n");               // Ausgabe über die Konsole
    }
}

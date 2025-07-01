// src/shell.c

// ##################################
// ## Includes
// ##################################
#include "shell.h"
#include "string_utils.h"
#include "uart.h"

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
static void process_command(char *buffer); // NEU: 'static', da jetzt intern
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
}

void shell_update() {
    unsigned char byte;
    // Versuche, ein Byte von der UART zu lesen
    if (uart_read_byte(&byte)) {
        // Enter wurde gedrückt
        if (byte == '\r') {
            uart_writeText("\n");
            input_buffer[input_buffer_pos] = '\0';

            if (input_buffer_pos > 0) {
                // shell_update ruft jetzt den Experten process_command auf
                process_command(input_buffer);
            }
            input_buffer_pos = 0;
            uart_writeText("> "); // Neuen Prompt anzeigen
        }
        // Backspace
        else if ((byte == 0x08 || byte == 0x7F) && input_buffer_pos > 0) {
            input_buffer_pos--;
            uart_writeText("\b \b");
        }
        // Normales, druckbares Zeichen
        else if (byte >= ' ' && byte <= '~' && input_buffer_pos < (INPUT_BUFFER_SIZE - 1)) {
            input_buffer[input_buffer_pos++] = byte;
            uart_writeByteBlocking(byte); // Echo
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
            uart_writeText("Error: Maximum number of variables reached!\n");
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
// ## Implementierung der öffentlichen Funktion (aus shell.h)
// ##################################

static void process_command(char *buffer) {
    char command[10];
    char var_name[MAX_VAR_NAME_LENGTH];
    char number_buffer[12];

    int i = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0') {
        if (i < 9) { command[i] = buffer[i]; }
        i++;
    }
    command[i] = '\0';

    if (strcmp_simple(command, "help") == 0) {
        uart_writeText("Commands:\n - set <name> <value>\n - print <expr>\n - version\n");
    } else if (strcmp_simple(command, "version") == 0) {
        uart_writeText("OhneBS v0.1.0-alpha\n");
    } else if (strcmp_simple(command, "set") == 0) {
        char* name_start = buffer + i + 1;
        int j = 0;
        while(name_start[j] != ' ' && name_start[j] != '\0') j++;
        strncpy_simple(var_name, name_start, j + 1);
        char* value_start = name_start + j + 1;
        int value = simple_atoi(value_start);
        set_variable(var_name, value);
        uart_writeText("OK.\n");
    } else if (strcmp_simple(command, "print") == 0) {
        char* expression = buffer + i + 1;
        bool success;
        int result = evaluate_expression(expression, &success);
        if (success) {
            uart_writeText(simple_itoa(result, number_buffer));
            uart_writeText("\n");
        } else {
            uart_writeText("Error: Invalid expression or variable not found.\n");
        }
    } else {
        uart_writeText("Unknown command: '");
        uart_writeText(buffer);
        uart_writeText("'\n");
    }
}

// Self-made definitions to replace standard libraries
#define true 1
#define false 0
typedef int bool;
#define NULL ((void*)0)

// Data structures for named variables
#define MAX_VARIABLES 50
#define MAX_VAR_NAME_LENGTH 16
typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    int value;
} NamedVariable;

// Buffer for UART input
#define INPUT_BUFFER_SIZE 256
char input_buffer[INPUT_BUFFER_SIZE];
unsigned int input_buffer_pos = 0;


// Global variables
NamedVariable variable_storage[MAX_VARIABLES];
unsigned int variable_count = 0;

// ######################################################
// ## ALL FUNCTION PROTOTYPES
// ######################################################

// UART & GPIO Prototypes
void uart_writeText(const char *buffer);
void uart_writeByteBlocking(unsigned char ch);

// String and number conversion
// FALSCH
int simple_atoi(const char *str);
void reverse_string(char *str, int length);
char* simple_itoa(int value, char *buffer);
int strcmp_simple(const char *s1, const char *s2);
int strncmp_simple(const char *s1, const char *s2, unsigned int n);
void strncpy_simple(char *dest, const char *src, unsigned int n);

// Variable management
NamedVariable* find_variable(const char* name);
void set_variable(const char* name, int value);
int get_variable(const char* name, bool* success);

// Command interpreter
void process_command(char *buffer);


// ######################################################
// ## IMPLEMENTATION OF HELPER FUNCTIONS
// ######################################################

/**
 * Converts a string to an int. Handles negative numbers.
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
 * Reverses a string.
 */
void reverse_string(char *str, int length) {
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

/**
 * Converts an int to a string.
 */
char* simple_itoa(int value, char *buffer) {
    int i = 0;
    bool is_negative = false;

    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    // Handle the special case of the minimum int value
    if (value == -2147483648) {
        strncpy_simple(buffer, "-2147483648", 12);
        return buffer;
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
 * Compares two strings.
 */
int strcmp_simple(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


/**
 * Compares the first 'n' characters of two strings.
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
 * Copies n characters or until the first space/null terminator.
 */
void strncpy_simple(char *dest, const char *src, unsigned int n) {
    while (n > 1 && *src != '\0' && *src != ' ') {
        *dest++ = *src++;
        n--;
    }
    *dest = '\0';
}

// ######################################################
// ## IMPLEMENTATION OF VARIABLE MANAGEMENT
// ######################################################

/**
 * Finds a variable by its name.
 */
NamedVariable* find_variable(const char* name) {
    for (unsigned int i = 0; i < variable_count; i++) {
        if (strcmp_simple(name, variable_storage[i].name) == 0) {
            return &variable_storage[i];
        }
    }
    return NULL;
}

/**
 * Sets the value of a variable or creates it.
 */
void set_variable(const char* name, int value) {
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

/**
 * Gets the value of a variable.
 */
int get_variable(const char* name, bool* success) {
    NamedVariable* var = find_variable(name);
    if (var != NULL) {
        if (success) *success = true;
        return var->value;
    }
    
    if (success) *success = false;
    uart_writeText("Error: Variable '");
    uart_writeText(name);
    uart_writeText("' not found.\n");
    return 0;
}

// ######################################################
// ## IMPLEMENTATION OF THE COMMAND INTERPRETER
// ######################################################

/**
 * Processes the user's command.
 */
void process_command(char *buffer) {
    char command[10];
    char var_name[MAX_VAR_NAME_LENGTH];
    char number_buffer[12]; 

    // Step 1: Extract the first word (the command)
    int i = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0') {
        if (i < 9) {
            command[i] = buffer[i];
        }
        i++;
    }
    command[i] = '\0';

    // Step 2: Compare the extracted command
    if (strcmp_simple(command, "help") == 0) {
        uart_writeText("Commands:\n - set <name> <value>\n - get <name>\n - version\n");
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
    } else if (strcmp_simple(command, "get") == 0) {
        char* name_start = buffer + i + 1;
        strncpy_simple(var_name, name_start, MAX_VAR_NAME_LENGTH);

        bool success;
        int value = get_variable(var_name, &success);

        if (success) {
            uart_writeText(simple_itoa(value, number_buffer));
            uart_writeText("\n");
        }
    } else {
        uart_writeText("Unknown command: '");
        uart_writeText(buffer);
        uart_writeText("'\n");
    }
}


// ######################################################
// ## GPIO AND UART CODE (UNCHANGED)
// ######################################################

enum {
    PERIPHERAL_BASE = 0xFE000000,
    GPFSEL0         = PERIPHERAL_BASE + 0x200000,
    GPSET0          = PERIPHERAL_BASE + 0x20001C,
    GPCLR0          = PERIPHERAL_BASE + 0x200028,
    GPPUPPDN0       = PERIPHERAL_BASE + 0x2000E4
};

enum {
    GPIO_MAX_PIN        = 53,
    GPIO_FUNCTION_OUT   = 1,
    GPIO_FUNCTION_ALT5  = 2,
    GPIO_FUNCTION_ALT3  = 7
};

enum {
    Pull_None = 0,
    Pull_Down = 2,
    Pull_Up   = 1
};

void mmio_write(long reg, unsigned int val) { *(volatile unsigned int *)reg = val; }
unsigned int mmio_read(long reg) { return *(volatile unsigned int *)reg; }

unsigned int gpio_call(unsigned int pin_number, unsigned int value, unsigned int base, unsigned int field_size, unsigned int field_max) {
    unsigned int field_mask = (1 << field_size) - 1;
  
    if (pin_number > field_max) return 0;
    if (value > field_mask) return 0; 

    unsigned int num_fields = 32 / field_size;
    unsigned int reg = base + ((pin_number / num_fields) * 4);
    unsigned int shift = (pin_number % num_fields) * field_size;

    unsigned int curval = mmio_read(reg);
    curval &= ~(field_mask << shift);
    curval |= value << shift;
    mmio_write(reg, curval);

    return 1;
}

unsigned int gpio_set       (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPSET0, 1, GPIO_MAX_PIN); }
unsigned int gpio_clear     (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPCLR0, 1, GPIO_MAX_PIN); }
unsigned int gpio_pull      (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPPUPPDN0, 2, GPIO_MAX_PIN); }
unsigned int gpio_function  (unsigned int pin_number, unsigned int value) { return gpio_call(pin_number, value, GPFSEL0, 3, GPIO_MAX_PIN); }

void gpio_useAsAlt5(unsigned int pin_number) {
    gpio_pull(pin_number, Pull_None);
    gpio_function(pin_number, GPIO_FUNCTION_ALT5);
}

void gpio_initOutputPinWithPullNone(unsigned int pin_number) {
    gpio_pull(pin_number, Pull_None);
    gpio_function(pin_number, GPIO_FUNCTION_OUT);
}

void gpio_setPinOutputBool(unsigned int pin_number, unsigned int onOrOff) {
    if (onOrOff) {
        gpio_set(pin_number, 1);
    } else {
        gpio_clear(pin_number, 1);
    }
}

// UART

enum {
    AUX_BASE        = PERIPHERAL_BASE + 0x215000,
    AUX_ENABLES     = AUX_BASE + 4,
    AUX_MU_IO_REG   = AUX_BASE + 64,
    AUX_MU_IER_REG  = AUX_BASE + 68,
    AUX_MU_IIR_REG  = AUX_BASE + 72,
    AUX_MU_LCR_REG  = AUX_BASE + 76,
    AUX_MU_MCR_REG  = AUX_BASE + 80,
    AUX_MU_LSR_REG  = AUX_BASE + 84,
    AUX_MU_CNTL_REG = AUX_BASE + 96,
    AUX_MU_STAT_REG = AUX_BASE + 100,
    AUX_MU_BAUD_REG = AUX_BASE + 104,
    AUX_UART_CLOCK  = 500000000,
    UART_MAX_QUEUE  = 16 * 1024
};

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK/(baud*8))-1)

unsigned char uart_output_queue[UART_MAX_QUEUE];
unsigned int uart_output_queue_write = 0;
unsigned int uart_output_queue_read = 0;

void uart_init() {
    mmio_write(AUX_ENABLES, 1); //enable UART1
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_MU_LCR_REG, 3); //8 bits
    mmio_write(AUX_MU_MCR_REG, 0);
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6); //disable interrupts
    mmio_write(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
    gpio_useAsAlt5(14);
    gpio_useAsAlt5(15);
    mmio_write(AUX_MU_CNTL_REG, 3); //enable RX/TX
}

unsigned int uart_isOutputQueueEmpty() {
    return uart_output_queue_read == uart_output_queue_write;
}

unsigned int uart_isReadByteReady()  { return mmio_read(AUX_MU_LSR_REG) & 0x01; }
unsigned int uart_isWriteByteReady() { return mmio_read(AUX_MU_LSR_REG) & 0x20; }

unsigned char uart_readByte() {
    while (!uart_isReadByteReady());
    return (unsigned char)mmio_read(AUX_MU_IO_REG);
}

void uart_writeByteBlockingActual(unsigned char ch) {
    while (!uart_isWriteByteReady()); 
    mmio_write(AUX_MU_IO_REG, (unsigned int)ch);
}

void uart_loadOutputFifo() {
    while (!uart_isOutputQueueEmpty() && uart_isWriteByteReady()) {
        uart_writeByteBlockingActual(uart_output_queue[uart_output_queue_read]);
        uart_output_queue_read = (uart_output_queue_read + 1) & (UART_MAX_QUEUE - 1);
    }
}

void uart_writeByteBlocking(unsigned char ch) {
    unsigned int next = (uart_output_queue_write + 1) & (UART_MAX_QUEUE - 1);

    while (next == uart_output_queue_read) uart_loadOutputFifo();

    uart_output_queue[uart_output_queue_write] = ch;
    uart_output_queue_write = next;
}

void uart_writeText(const char *buffer) {
    while (*buffer) {
        if (*buffer == '\n') uart_writeByteBlocking('\r');
        uart_writeByteBlocking(*buffer++);
    }
}

void uart_update() {
    uart_loadOutputFifo();

    if (uart_isReadByteReady()) {
        unsigned char ch = uart_readByte();

        if (ch == '\r') {
            uart_writeText("\n");
            input_buffer[input_buffer_pos] = '\0';
            
            if (input_buffer_pos > 0) {
                process_command(input_buffer);
            }
            
            input_buffer_pos = 0;
        } else if (ch == 0x08 || ch == 0x7F) { // Backspace
            if (input_buffer_pos > 0) {
                input_buffer_pos--;
                uart_writeText("\b \b");
            }
        } else {
            if (input_buffer_pos < (INPUT_BUFFER_SIZE - 1)) {
                input_buffer[input_buffer_pos++] = ch;
                uart_writeByteBlocking(ch); // Echo
            }
        }
    }
}

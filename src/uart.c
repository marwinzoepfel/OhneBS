#include "uart.h"
#include "gpio.h" // Wird für gpio_useAsAlt5 und PERIPHERAL_BASE benötigt

//==================================================================
// Private Defines und globale Variablen
// 'static' macht sie nur in dieser Datei sichtbar.
//==================================================================
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

static unsigned char uart_output_queue[UART_MAX_QUEUE];
static unsigned int uart_output_queue_write = 0;
static unsigned int uart_output_queue_read = 0;

//==================================================================
// Private Hilfsfunktionen
//==================================================================

static bool uart_isOutputQueueEmpty() {
    return uart_output_queue_read == uart_output_queue_write;
}

static bool uart_isWriteByteReady() {
    return mmio_read(AUX_MU_LSR_REG) & 0x20;
}

static void uart_writeByteBlockingActual(unsigned char ch) {
    while (!uart_isWriteByteReady());
    mmio_write(AUX_MU_IO_REG, (unsigned int)ch);
}

static void uart_loadOutputFifo() {
    while (!uart_isOutputQueueEmpty() && uart_isWriteByteReady()) {
        uart_writeByteBlockingActual(uart_output_queue[uart_output_queue_read]);
        uart_output_queue_read = (uart_output_queue_read + 1) & (UART_MAX_QUEUE - 1);
    }
}


//==================================================================
// Öffentliche Funktionen (deklariert in uart.h)
//==================================================================

void uart_init() {
    mmio_write(AUX_ENABLES, 1); //enable UART1
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_MU_LCR_REG, 3); //8 bits
    mmio_write(AUX_MU_MCR_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6); //disable interrupts
    mmio_write(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
    gpio_useAsAlt5(14);
    gpio_useAsAlt5(15);
    mmio_write(AUX_MU_CNTL_REG, 3); //enable RX/TX
}

void uart_writeByteBlocking(unsigned char ch) {
    unsigned int next = (uart_output_queue_write + 1) & (UART_MAX_QUEUE - 1);

    // Warte, bis Platz in der Queue ist, währenddessen die FIFO laden
    while (next == uart_output_queue_read) {
        uart_loadOutputFifo();
    }

    uart_output_queue[uart_output_queue_write] = ch;
    uart_output_queue_write = next;
}

void uart_writeText(const char *buffer) {
    while (*buffer) {
        // Konvertiere \n (Newline) zu \r\n (Carriage Return + Newline)
        if (*buffer == '\n') {
            uart_writeByteBlocking('\r');
        }
        uart_writeByteBlocking(*buffer++);
    }
    // Lade die FIFO, damit der Text auch wirklich gesendet wird
    uart_loadOutputFifo();
}

/**
 * Prüft, ob ein Byte zum Lesen bereitsteht.
 * Wenn ja, wird es in den 'byte'-Pointer geschrieben und 'true' zurückgegeben.
 * Diese Funktion blockiert nicht.
 */
bool uart_read_byte(unsigned char* byte) {
    // Bearbeite die Sende-Queue, auch wenn wir auf Empfang prüfen
    uart_loadOutputFifo();

    // Prüfe das "data ready" Bit im Line Status Register
    if (mmio_read(AUX_MU_LSR_REG) & 0x01) {
        // Lese das Zeichen aus dem IO-Register
        *byte = (unsigned char)mmio_read(AUX_MU_IO_REG);
        return true;
    }

    return false;
}

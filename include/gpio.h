#ifndef GPIO_H
#define GPIO_H

#define PERIPHERAL_BASE 0xFE000000
// MMIO (wird von GPIO und UART gebraucht, passt gut hierher)
void mmio_write(long reg, unsigned int val);
unsigned int mmio_read(long reg);

// GPIO Funktionen
void gpio_useAsAlt5(unsigned int pin_number);
void gpio_initOutputPinWithPullNone(unsigned int pin_number);
void gpio_setPinOutputBool(unsigned int pin_number, unsigned int onOrOff);

#endif // GPIO_H

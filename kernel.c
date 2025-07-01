#include "io.h"


void main()
{

  uart_init();
  uart_writeText("Hello to my os, write help for help!\n");
  while (1) uart_update();
}


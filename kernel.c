#include "io.h"


void main()
{

  uart_init();
  uart_writeText("Welcome to OhneBS v0.1.0-alpha, type help for help!\n");
  while (1) uart_update();
}


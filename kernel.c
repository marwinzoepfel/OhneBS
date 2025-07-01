#include "io.h"


void main()
{

  
  // A few seconds delay to start my PuTTY system
  for (volatile int i = 0; i < 500000; i++); 



  uart_init();
  uart_writeText("Welcome to OhneBS v0.1.0-alpha, type help for help!\n");
  while (1) uart_update();
}


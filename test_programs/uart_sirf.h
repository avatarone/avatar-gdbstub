#ifndef _UART_SIRF_H
#define _UART_SIRF_H

#include <stdint.h>

#define UART_BASE ((volatile uint16_t *) 0x80030000)

static int uart_getc()  {
	while (!(UART_BASE[1] & (1 << 0x04)));
	return UART_BASE[3];
}

//static __attribute__((__naked__))

static int uart_putc(int c)  {
	while (UART_BASE[1] & (1 << 0x6));
	UART_BASE[2] = c;
	
	return 0;
}
#endif  /* _UART_SIRF_H */

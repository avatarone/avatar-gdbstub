#ifndef _UART_COMMON_H
#define _UART_COMMON_H

#include <stdint.h>

static int uart_getc();
static int uart_putc(int c);

static int uart_puts(const char * str)  {
	while (*str)  {
		uart_putc(*str);
		str += 1;
	}
	
	return 0;
}

static void uart_puthex(uint32_t val)  {
	int i;
	uint32_t digit;
	
	uart_puts("0x");
	
	for (i = 7; i >= 0; i--)  {
		digit = (val >> (4 * i)) & 0xf;
		if (digit >= 0 && digit <= 9)  {
			uart_putc(digit + '0');
		}
		else {
			uart_putc(digit + 'A' - 10);
		}
	}
}

#endif /* _UART_COMMON_H */

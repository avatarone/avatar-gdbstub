#ifndef _UART_COMMON_H
#define _UART_COMMON_H

#include <stdint.h>

static int uart_getc();
static int uart_putc(int c);
static void uart_puthexn(uint32_t val, unsigned digits);

int uart_puts(const char * str)  {
	while (*str)  {
		uart_putc(*str);
		str += 1;
	}
	
	return 0;
}

void uart_puthexn(uint32_t val, unsigned digits)  {
	int i;
	uint32_t digit;
	
	uart_puts("0x");
	
	for (i = digits - 1; i >= 0; i--)  {
		digit = (val >> (4 * i)) & 0xf;
		if (digit >= 0 && digit <= 9)  {
			uart_putc(digit + '0');
		}
		else {
			uart_putc(digit + 'A' - 10);
		}
	}
}

#define uart_puthex(val) uart_puthexn(val, 8)

#endif /* _UART_COMMON_H */

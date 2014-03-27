#ifndef _STARTUP_INFORAD_K0_H
#define _STARTUP_INFORAD_K0_H

#include <stdint.h>

static int uart_puts(const char * str);
static void uart_puthexn(uint32_t val, unsigned digits);
//static void uart_puthexn(uint32_t num, unsigned x);

__attribute__((__naked__)) void _start()  {
          /* TODO: Put startup code here, e.g., stack initialization,
             and then call main. */
    __asm__(
	".section .init         \n"
    "b _reset               \n"
	"b _und_exception       \n"
	"b _svc_exception       \n"
	"b _pab_exception       \n"
	"b _dab_exception       \n"
	"nop                    \n"
	"b _irq_exception       \n"
	"b _fiq_exception       \n"
	".section .text         \n"
"_und_exception:        \n"
	"mov r0, #1             \n"
	"b _exception           \n"
"_svc_exception:        \n"
	"mov r0, #2             \n"
	"b _exception           \n"
"_pab_exception:        \n"
	"mov r0, #1             \n"
	"b _exception           \n"
"_dab_exception:        \n"
	"mov r0, #1             \n"
	"b _exception           \n"				
"_irq_exception:        \n"
	"mov r0, #1             \n"
	"b _exception           \n"
"_fiq_exception:        \n"
	"mov r0, #1             \n"
	"b _exception           \n"
"_exception:           \n"
	"mov r1, lr             \n"
	"ldr sp, =0x60017ffc    \n" 
	"bl exception_handler   \n"
	"b .                    \n"
"_reset:                \n"
    "ldr sp, =0x60017ffc    \n"
	"bl main                \n"
	"b .                    \n");
}

void exception_handler(unsigned exception_index, uint32_t lr)
{
	uart_puts("Exception ");
	uart_puthexn(exception_index, 1);
	uart_puts(" at ");
	uart_puthexn(lr, 8);
	uart_puts(" occured.\n");
	
	while (1);
}

#endif /* _STARTUP_INFORAD_K0_H */



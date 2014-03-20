#include "uart_common.h"

uint32_t get_didr() {
	uint32_t didr;
	
	asm("mrc p14, 0, %[didr], c0, c0, 0" : [didr] "=r" (didr));
	return didr;
}



void test_hw_breakpoint()
{
	
}

int main()  {
	uint32_t didr = cp14_get_didr();
	
	unsigned debug_ver = (didr >> 16) & 0xf;
	unsigned num_brp_context = ((didr >> 20) & 0xf) + 1;
	unsigned num_brp = (didr >> 24) & 0xf;
	unsigned num_wrp = ((didr >> 28) & 0xf) + 1;
	
	if (num_brp > 0) {
		num_brp += 1;
	}
	
	uart_puts("Debug version: "); uart_puthexn(debug_ver, 1); uart_puts("\n");
	uart_puts("#BRPs with context ID: "); uart_puthexn(num_brp_context, 2); uart_puts("\n");
	uart_puts("#BRPs: "); uart_puthexn(num_brp, 2); uart_puts("\n");
	uart_puts("#WRPs: "); uart_puthexn(num_wrp, 2); uart_puts("\n");
	uart_puts("Finished debug register identification, now testing hardware breakpoint\n");
	
	test_hw_breakpoint();
	
	
	while (1);
}



#include "uart_common.h"
#include "coprocessor.h"

void test_hw_breakpoint()
{
	
}

int main()  {
	uint32_t didr = cp14_get_didr();
	
	unsigned debug_ver = (didr >> 16) & 0xf;
	unsigned num_brp_context = ((didr >> 20) & 0xf) + 1;
	unsigned num_brp = (didr >> 24) & 0xf;
	unsigned num_wrp = ((didr >> 28) & 0xf) + 1;
	unsigned rev = didr & 0xf;
	unsigned var = (didr >> 4) & 0xf;
	
	if (num_brp > 0) {
		num_brp += 1;
	}
	
	uart_puts("Debug version: "); uart_puthexn(debug_ver, 1); uart_puts("\n");
	uart_puts("#BRPs with context ID: "); uart_puthexn(num_brp_context, 2); uart_puts("\n");
	uart_puts("#BRPs: "); uart_puthexn(num_brp, 2); uart_puts("\n");
	uart_puts("#WRPs: "); uart_puthexn(num_wrp, 2); uart_puts("\n");
	uart_puts("Variant number: "); uart_puthexn(var, 1); uart_puts("\n");
	uart_puts("Revision number: "); uart_puthexn(rev, 1); uart_puts("\n");
	uart_puts("Finished debug register identification, now testing hardware breakpoint\n");
	
	test_hw_breakpoint();
	
	
	while (1);
}



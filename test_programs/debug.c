#include "uart_common.h"
#include "coprocessor.h"

void break_in_this_function()
{
	uart_puts("Breakpoint not hit\n");
}

void test_hw_breakpoint()
{
	unsigned i;
	
	cp14_set_dscr(0);
	cp14_set_vcr(0);
	for (i = 0; i < 16; i++)  {
		cp14_set_bcr(i, 0);
		cp14_set_wcr(i, 0);
	}
	
	cp14_set_bvr(0, (uint32_t) break_in_this_function);
	cp14_set_bcr(0, 
		(1 << 0) /* Breakpoint enable */
		| (0b11 << 1) /* Supervisor and user mode */
		| (0b1111 << 5) /* Trigger BP if any of the four instruction bytes is read */
		| (0 << 20) /* Breakpoint linking disabled */
		| (0b00 << 21) /* Instruction virtual address match */);
	cp14_set_dscr(1 << 15);
	
	break_in_this_function();
	
	uart_puthex(cp14_get_bvr(0));
}

static uint32_t get_didr()
{
	uint32_t didr;
	
	asm("mrc 14, 0, %[didr], c0, c0, 0" : [didr] "=r" (didr));
	return didr;
}

__attribute__((__naked__)) uint32_t get_didr2()
{
	asm("mrc 14, 0, r0, c0, c0, 0  \n"
		"bx lr");
	return 0;
}

int main()  {
	uint32_t didr = cp14_get_didr();
	uint32_t didr2 = get_didr();
	uint32_t didr3 = get_didr2();
	
	unsigned debug_ver = (didr >> 16) & 0xf;
	unsigned num_brp_context = ((didr >> 20) & 0xf) + 1;
	unsigned num_brp = (didr >> 24) & 0xf;
	unsigned num_wrp = ((didr >> 28) & 0xf) + 1;
	unsigned rev = didr & 0xf;
	unsigned var = (didr >> 4) & 0xf;
	
	if (num_brp > 0) {
		num_brp += 1;
	}
	
	uart_puts("DIDR1 register value: "); uart_puthex(didr); uart_puts("\n");
	uart_puts("DIDR2 register value: "); uart_puthex(didr2); uart_puts("\n");
	uart_puts("DIDR3 register value: "); uart_puthex(didr3); uart_puts("\n");
	uart_puts("Debug version: "); uart_puthexn(debug_ver, 1); uart_puts("\n");
	uart_puts("#BRPs with context ID: "); uart_puthexn(num_brp_context, 2); uart_puts("\n");
	uart_puts("#BRPs: "); uart_puthexn(num_brp, 2); uart_puts("\n");
	uart_puts("#WRPs: "); uart_puthexn(num_wrp, 2); uart_puts("\n");
	uart_puts("Variant number: "); uart_puthexn(var, 1); uart_puts("\n");
	uart_puts("Revision number: "); uart_puthexn(rev, 1); uart_puts("\n");
	uart_puts("Finished debug register identification, now testing hardware breakpoint\n");
	
	test_hw_breakpoint();
	
	uart_puts("Still alive\n");
	
	
	while (1);
}



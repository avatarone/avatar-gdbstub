#include "uart_common.h"


int main()  
{
	uart_puts("Program started\n");
		asm(".long 0xe12fff7f");
//	asm(".long 0xe7f001f0");
	
	uart_puts("Xrogram finished\n");
//	asm(".long 0xe7f001f0");
	//For thumb use 0xde01
	
	while (1);
}



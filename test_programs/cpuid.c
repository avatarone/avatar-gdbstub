#include "uart_common.h"

uint32_t get_cpuid_reg() 
{
	uint32_t cpuid_reg;
	
	asm("mrc p15, 0, %[cpuid_reg], c0, c0, 0" : [cpuid_reg] "=r" (cpuid_reg));
	return cpuid_reg;
}

const char * get_implementor(unsigned vendor_id) {
	switch(vendor_id) {
		case 0x41: return "ARM Ltd";
		case 0x44: return "DEC";
		case 0x4d: return "Motorola - Freescale";
		case 0x56: return "Marvell Semiconductor";
		case 0x69: return "Intel";
		default: return "Unknown vendor";
	}
}

void identify_post_arm7_processor(uint32_t cpuid)
{
	uart_puts("Post-ARMv6 processor\n");
}

void identify_pre_armv4_processor(uint32_t cpuid)
{
	uart_puts( "Pre-ARMv4 processor\n");
}

void identify_arm7_processor(uint32_t cpuid)
{
	unsigned implementor_id = (cpuid >> 24) & 0xff;
	const char * implementor = get_implementor(implementor_id);
	unsigned architecture_id = (cpuid >> 23) & 0x1;
	const char * architecture = architecture_id ? "v4T" : "v3";
	unsigned variant_id = (cpuid >> 16) & 0x7f;
	unsigned ppn = (cpuid >> 4) & 0xfff;
	unsigned revision = cpuid & 0xf;
	
	uart_puts("CPUID register value: "); uart_puthexn(cpuid, 8); uart_puts("\n");
	uart_puts("Implementor: "); uart_puts(implementor); uart_puts(" ("); uart_puthexn(implementor_id, 2); uart_puts(")\n");
	uart_puts("Archictecture: "); uart_puts(architecture); uart_puts("\n");
	uart_puts("Variant: "); uart_puthexn(variant_id, 2); uart_puts("\n");
	uart_puts("Primary part nr: "); uart_puthexn(ppn, 3); uart_puts("\n");
	uart_puts("Revision: "); uart_puthexn(revision, 1); uart_puts("\n");
}

int main()  
{
	uint32_t cpuid;
	
	uart_puts("Welcome to the CPUID program\n");
	
	cpuid = get_cpuid_reg();
	
	uart_puts("CPUID register has been fetched\n");
	
	if (cpuid & (1 << 19)) {
		identify_post_arm7_processor(cpuid);
	}
	else if (((cpuid >> 12) & 0xf) == 0) {
		identify_pre_armv4_processor(cpuid);
	}
	else if (((cpuid >> 12) & 0xf) == 7) {
		identify_arm7_processor(cpuid);
	}
	else {
		uart_puts("Unknown ARM processor\n");
	}

	uart_puts("CPU identification finished\n");
	while (1);
}



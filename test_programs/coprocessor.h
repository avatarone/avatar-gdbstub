#ifndef _COPROCESSOR_REGISTER_H
#define _COPROCESSOR_REGISTER_H

typedef uint32_t (*mrc_func_t)(void);
typedef void (*mcr_func_t)(uint32_t);

static uint32_t get_coprocessor_register(unsigned cp_num, unsigned opcode_1, unsigned cr_n, unsigned cr_m, unsigned opcode_2)
{
	uint32_t buf[2];
	mrc_func_t mrc_func = (mrc_func_t) buf;
	
	buf[0] = (0xe << 28) //Condition
		     | (0xe << 24) //Fixed part
		     | ((opcode_1 & 0x7) << 21) 
			 | (1 << 20) //Fixed part
			 | ((cr_n & 0xf) << 16)
			 | (0 << 12) //R0
			 | ((cp_num & 0xf) << 8)
			 | ((opcode_2 & 0x7) << 5)
			 | (1 << 4) //Fixed part
			 | (cr_m & 0xf);
	buf[1] = 0xe12fff1e; //bx lr
		
	return mrc_func();
}

static void set_coprocessor_register(unsigned cp_num, unsigned opcode_1, unsigned cr_n, unsigned cr_m, unsigned opcode_2, uint32_t val)
{
	uint32_t buf[2];
	mcr_func_t mcr_func = (mcr_func_t) buf;
	
	buf[0] = (0xe << 28) //Condition
		     | (0xe << 24) //Fixed part
		     | ((opcode_1 & 0x7) << 21) 
			 | (0 << 20) //Fixed part
			 | ((cr_n & 0xf) << 16)
			 | (0 << 12) //R0
			 | ((cp_num & 0xf) << 8)
			 | ((opcode_2 & 0x7) << 5)
			 | (1 << 4) //Fixed part
			 | (cr_m & 0xf);
	buf[1] = 0xe12fff1e; //bx lr
		
	mcr_func(val);
}

#define cp15_get_midr() get_coprocessor_register(15, 0, 0, 0, 0) //Main Identification register, CPUID

#define cp14_get_didr() get_coprocessor_register(14, 0, 0, 0b0000, 0b000)
#define cp14_get_dscr() get_coprocessor_register(14, 0, 0, 0b0001, 0b000)
#define cp14_get_dtr() get_coprocessor_register(14, 0, 0, 0b0101, 0b000)
#define cp14_get_wfar() get_coprocessor_register(14, 0, 0, 0b0110, 0b000)
#define cp14_get_vcr() get_coprocessor_register(14, 0, 0, 0b0111, 0b000)
#define cp14_get_bvr(n) get_coprocessor_register(14, 0, 0, (n & 0xf), 0b100)
#define cp14_get_bcr(n) get_coprocessor_register(14, 0, 0, (n & 0xf), 0b101)
#define cp14_get_wvr(n) get_coprocessor_register(14, 0, 0, (n & 0xf), 0b110)
#define cp14_get_wcr(n) get_coprocessor_register(14, 0, 0, (n & 0xf), 0b111)

#define cp14_set_dscr(v) set_coprocessor_register(14, 0, 0, 0b0001, 0b000, v)
#define cp14_set_dtr(v) set_coprocessor_register(14, 0, 0, 0b0101, 0b000, v)
#define cp14_set_vcr(v) set_coprocessor_register(14, 0, 0, 0b0111, 0b000, v)
#define cp14_set_bvr(n, v) set_coprocessor_register(14, 0, 0, (n & 0xf), 0b100, v)
#define cp14_set_bcr(n, v) set_coprocessor_register(14, 0, 0, (n & 0xf), 0b101, v)
#define cp14_set_wvr(n, v) set_coprocessor_register(14, 0, 0, (n & 0xf), 0b110, v)
#define cp14_set_wcr(n, v) set_coprocessor_register(14, 0, 0, (n & 0xf), 0b111, v)

#endif /* _COPROCESSOR_REGISTER_H */
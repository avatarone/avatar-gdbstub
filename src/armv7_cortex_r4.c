/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */
#include <stdint.h>

#include "ArmGdbHelpers.h"
#include "RegisterMap.h"
#include "ArmRegisterMap.h"
#include "Assert.h"
#include "armv7_cortex_r4.h"

uint32_t armv7_cortex_r4_read_debug_register(StubState *state, uint32_t reg_num);
uint32_t armv7_cortex_r4_read_Debug_ROM_Address_Register(StubState *state);
uint32_t armv7_cortex_r4_read_Debug_Self_Address_Offset_Register(StubState *state);
uint32_t armv7_cortex_r4_read_debug_register(StubState *state, uint32_t reg_num);
void armv7_cortex_r4_write_debug_register(StubState *state, uint32_t reg_num, uint32_t value);
volatile void * armv7_cortex_r4_get_debug_registers_base(StubState *state);
void armv7_cortex_r4_unlock_debug_registers(StubState *state);
void armv7_cortex_r4_lock_debug_registers(StubState *state);
int armv7_cortex_r4_are_debug_register_enabled(StubState *state);

void armv7_cortex_r4_enable_monitor_mode(StubState *state)
{
/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0363e/Cegfhaac.html */
	uint32_t tmp;
	asm volatile ("mrc p14, 0, %[r], c0, c1, 0\n" : [r] "=r" (tmp));
	tmp |= (0x1 << 15);
	asm volatile ("mcr p14, 0, %[r], c0, c1, 0\n" : /* no output */ : [r] "r" (tmp));
}

void
armv7_cortex_r4_set_hw_breakpoint(StubState *state, uint32_t addr, uint32_t break_num)
{
	/* 11.11.2. Programming breakpoints and watchpoints of the TRM */
	/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0363e/Ceggchda.html
	 * */
	/* TODO move break_num in StubState */

	/* Example 11.7. Setting a simple breakpoint */
	/* break_num from 0 to 7 */

	armv7_cortex_r4_unlock_debug_registers(state);
	break_num &= 0x7;
	write_debug_register(80 + break_num, 0x0);
	write_debug_register(64 + break_num, addr & 0xFFFFFFC);
	write_debug_register(80 + break_num, 7 | (15 << 5));
}

uint32_t
armv7_cortex_r4_get_Authentication_Status_Register(StubState *state)
{
	return read_debug_register(0xFB8 >> 2);
}

void
armv7_cortex_r4_set_hw_watchpoint(StubState *state, uint32_t addr, uint32_t watch_num, uint32_t size)
{
	/*
	 * Example 11.8. Setting a simple aligned watchpoint
	 */

	/* watchpiont from 0 to 7 */
	/* assumptions: addr is aligned */
	uint32_t byte_address_select;

	armv7_cortex_r4_unlock_debug_registers(state);
	watch_num &= 0x7;
	write_debug_register(112 + watch_num, 0);
	write_debug_register(96 + watch_num, addr & 0xFFFFFF8);

	switch (size) {
	case 1:
		byte_address_select = (1 << (addr & 7));
		break;
	case 2:
		byte_address_select = (3 << (addr & 6));
		break;
	case 4:
		byte_address_select = (15 << (addr & 4));
		break;
	case 8:
		byte_address_select = 255;
	default:
		byte_address_select = (15 << (addr & 4));
		return;
	}

	write_debug_register(112 + watch_num, 23 | (byte_address_select << 5));
}

void
armv7_cortex_r4_unlock_debug_registers(StubState *state)
{
	/*
	 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0363e/Babfcfig.html
	 */
	/* XXX: this might block */
	while (!armv7_cortex_r4_are_debug_register_enabled(state))
		write_debug_register((0xFB0 >> 2), 0xC5ACCE55);
}

void
armv7_cortex_r4_lock_debug_registers(StubState *state)
{
	/*
	 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0363e/Babfcfig.html
	 */
	write_debug_register((0xFB0 >> 2), 0xABCD1234);
}

int
armv7_cortex_r4_are_debug_register_enabled(StubState *state)
{
	/* Lock Status Register */
	return !(read_debug_register(0xFB4 >> 2) & 0x2);
}


uint32_t
armv7_cortex_r4_read_Debug_ROM_Address_Register(StubState *state)
{
	uint32_t ret;
	asm volatile ("mrc p14, 0, %[r], c1, c0, 0\n" : [r] "=r" (ret));
	return ret;
}

uint32_t
armv7_cortex_r4_read_Debug_Self_Address_Offset_Register(StubState *state)
{
	uint32_t ret;
	asm volatile ("mrc p14, 0, %[r], c2, c0, 0\n" : [r] "=r" (ret));
	return ret;
}

uint32_t
armv7_cortex_r4_read_debug_register(StubState *state, uint32_t reg_num)
{
	volatile uint32_t *debug_base = armv7_cortex_r4_get_debug_registers_base(state);
	return debug_base[reg_num];
}

void
armv7_cortex_r4_write_debug_register(StubState *state, uint32_t reg_num, uint32_t value)
{
	volatile uint32_t *debug_base = armv7_cortex_r4_get_debug_registers_base(state);
	debug_base[reg_num] = value;
}

volatile void *
armv7_cortex_r4_get_debug_registers_base(StubState *state)
{
	uint32_t rom = armv7_cortex_r4_read_Debug_ROM_Address_Register(state);
	uint32_t off = armv7_cortex_r4_read_Debug_Self_Address_Offset_Register(state);
	return (volatile void *)((rom&0xfffff000) + (off&0xfffff000));
}

void
armv7_cortex_r4_flush_data_cache_by_mva(uint32_t address)
{
	/* this function takes a MVA parameter */
	asm("dsb    \n"
			"mcr p15, 0, %[addr], c7, c14, 1    \n"
			"dsb\n" :: [addr] "r" (address & 0xFFFFFFF0));
}

void
armv7_cortex_r4_flush_data_cache()
{
	int i, j;

#define no_sets 8/* we don't know for sure, we should check the documentation */
	for (i = 0; i < (1<<no_sets); ++i) {
#define no_ways 2
		for (j = 0; j < (1<<no_ways); ++j) {
			asm("dsb    \n"
					"mcr p15, 0, %[addr], c7, c10, 2\n" :: [addr] "r" ((i<<5) | (j<<30)));
		}
	}
	asm("dsb\n");
}

void
armv7_cortex_r4_invalidate_data_cache(void)
{
	/* invalidate the isn cache as well?
	 */
	/* XXX: this triggers a data abort */
#warning "invalidate data cache is not implemented"
	/*
	asm(
			"mcr p15, 0, %[zero], c15, c5, 0     \n"
			"dsb         \n"
			"isb         \n" :: [zero] "r" (0));
			*/
}

void
armv7_cortex_r4_invalidate_instruction_cache(void)
{
	asm(
			"mcr p15, 0, %[zero], c7, c5, 0\n"
			"dsb\n"
			"isb\n"  :: [zero] "r" (0));
}

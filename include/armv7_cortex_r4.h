/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */
#ifndef _ARMV7_CORTEX_R4_H
#define _ARMV7_CORTEX_R4_H 1

#include <stdint.h>

#include "StubState.h"

uint32_t armv7_cortex_r4_read_debug_register(StubState *state, uint32_t reg_num);
uint32_t armv7_cortex_r4_read_Debug_ROM_Address_Register(StubState *state);
uint32_t armv7_cortex_r4_read_Debug_Self_Address_Offset_Register(StubState *state);
uint32_t armv7_cortex_r4_read_debug_register(StubState *state, uint32_t reg_num);
void armv7_cortex_r4_write_debug_register(StubState *state, uint32_t reg_num, uint32_t value);
volatile void * armv7_cortex_r4_get_debug_registers_base(StubState *state);
void armv7_cortex_r4_unlock_debug_registers(StubState *state);
void armv7_cortex_r4_lock_debug_registers(StubState *state);
int armv7_cortex_r4_are_debug_register_enabled(StubState *state);
void armv7_cortex_r4_set_hw_watchpoint(StubState *state, uint32_t addr, uint32_t watch_num, uint32_t size);
void armv7_cortex_r4_enable_monitor_mode(StubState *state);
void armv7_cortex_r4_set_hw_breakpoint(StubState *state, uint32_t addr, uint32_t break_num);

void armv7_cortex_r4_invalidate_instruction_cache(void);
void armv7_cortex_r4_flush_data_cache(void);
void armv7_cortex_r4_invalidate_data_cache(void);
void armv7_cortex_r4_flush_data_cache_by_mva(uint32_t address);

#define flush_data_cache armv7_cortex_r4_flush_data_cache
#define invalidate_instruction_cache armv7_cortex_r4_invalidate_instruction_cache
#define invalidate_data_cache armv7_cortex_r4_invalidate_data_cache
#define flush_data_cache_by_mva armv7_cortex_r4_flush_data_cache_by_mva

#define write_debug_register(r, v) armv7_cortex_r4_write_debug_register(state, (r), (v))
#define read_debug_register(r) armv7_cortex_r4_read_debug_register(state, (r))
#define set_hw_breakpoint(state, addr, no) do {\
	armv7_cortex_r4_enable_monitor_mode(state);\
	armv7_cortex_r4_set_hw_breakpoint(state, addr, no);\
} while (0)
#define set_hw_watchpoint(state, addr, no) do {\
	armv7_cortex_r4_enable_monitor_mode(state);\
	armv7_cortex_r4_set_hw_watchpoint(state, addr, no, 4);\
} while (0)

#endif

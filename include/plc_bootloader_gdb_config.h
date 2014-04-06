/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */
#ifndef _PLC_BOOTLOADER_GDB_CONFIG_H
#define _PLC_BOOTLOADER_GDB_CONFIG_H

#define UART_PL011_BASE ((volatile uint32_t *) 0xfffb8000)

#include <stdint.h>

static inline void led_set(uint32_t val)
{
	*(volatile uint32_t *)(0xfffbb154) = val;
}

static inline uint32_t led_get(void)
{
	return *(volatile uint32_t *)(0xfffbb154);
}

static inline void led_toggle(uint32_t led_id)
{
	uint32_t v = led_get();
	v ^= 1 << (led_id<<4);
	led_set(v);
}

static inline void led_on(uint32_t led_id)
{
	led_set(led_get() | (1 << (led_id<<4)));
}

static inline void led_off(uint32_t led_id)
{
	led_set(led_get() & (~(1 << (led_id<<4))));
}

static inline void led_blink(uint32_t led_id)
{
	led_set(0x1f320000 | ((led_get() | (1 << (led_id<<4))) & 0x0000ffff));
}

#define LED_RED 0
#define LED_GREEN 1
#define LED_ORANGE0 2
#define LED_ORANGE1 3

#define signal_debug_enter() do {led_blink(LED_RED); } while (0)
#define signal_debug_exit() do {led_off(LED_RED); } while (0)

#define WATCHDOG_EXCITE do {*((volatile uint32_t *)0xFFFBB120) = 0x967EA5C3;} while (0)

#define HAS_LOW_LEVEL_HELPERS 1

#define CONFIG_CHECKSUM 1

#include "GdbHostInterface.h"
#include "ArmRegisterMap.h"
#include "Serial.h"

#endif

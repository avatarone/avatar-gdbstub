/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */
#include <stdint.h>

#include "Serial.h"
#include "SerialIO.h"
#include "gdb_utils.h"

void
print_uint8(uint8_t val)
{
	char c = nibble_to_hex_char(val >> 4);
	Serial_write_byte(c);
	c = nibble_to_hex_char(val);
	Serial_write_byte(c);
}

void
print_uint32(uint32_t val)
{
	print_uint8(val >> 24);
	print_uint8(val >> 16);
	print_uint8(val >> 8);
	print_uint8(val);
}

void
print_char(char c)
{
	Serial_write_byte(c);
}


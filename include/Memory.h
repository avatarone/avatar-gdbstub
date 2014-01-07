/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>

#include "StubState.h"

typedef uint32_t value_t; //This should be the type of the biggest value that can be read
typedef uint32_t address_t;//This type should be able to hold a memory address

#define SIZE_CHAR 1
#define SIZE_SHORT 2
#define SIZE_LONG 4

/**
 * Read a typed (byte, short, long) value from memory.
 */
value_t Memory_read_typed(StubState *state, address_t address, unsigned size);

/**
 * Write a typed (byte, short, long) value to memory.
 */
void Memory_write_typed(StubState *state, address_t address, unsigned size, value_t value);

/**
 * Read untyped (byte array) data from memory.
 */
void Memory_read_untyped(StubState *state, address_t address, uint8_t *buffer, unsigned length);


/**
 * Write untyped (byte array) data to memory.
 */
void Memory_write_untyped(StubState *state, address_t address, const uint8_t *buffer, unsigned length);

/**
 * Test or check in memory map if this is a valid address.
 */
int Memory_is_valid_address(StubState *state, address_t address);






#endif /* _MEMORY_H */

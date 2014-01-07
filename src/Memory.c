/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "Memory.h"
#include "Assert.h"

int Memory_is_valid_address(StubState* state, address_t address)
{
    return 1;
}
    

value_t Memory_read_typed(StubState *state, address_t address, unsigned size)
{
    if (Memory_is_valid_address(state, address))
    {
        switch (size) 
        {
            case SIZE_CHAR:
                return *((uint8_t *) address);
            case SIZE_SHORT:
                return *((uint16_t *) address);
            case SIZE_LONG:
                return *((uint32_t *) address);
            default:
                assert(0 && "Unknown memory type size");
                break;
        }
    }
    
    //Error case
    return 0xfefefefe;
}

/**
 * Write a typed (byte, short, long) value to memory.
 */
void Memory_write_typed(StubState *state, address_t address, unsigned size, value_t value)
{
    if (Memory_is_valid_address(state, address))
    {
        switch (size) 
        {
            case SIZE_CHAR:
                *((uint8_t *) address) = value;
                break;
            case SIZE_SHORT:
                *((uint16_t *) address) = value;
                break;
            case SIZE_LONG:
                *((uint32_t *) address) = value;
                break;
            default:
                assert(0 && "Unknown memory type size");
                break;
        }
    }
}


/**
 * Read untyped (byte array) data from memory.
 */
void Memory_read_untyped(StubState *state, address_t address, uint8_t *buffer, unsigned length)
{
    if (Memory_is_valid_address(state, address))
    {
        unsigned i;
        for (i = 0; i < length; i++)
            buffer[i] = ((uint8_t *) address)[i];
    }
}


/**
 * Write untyped (byte array) data to memory.
 */
void Memory_write_untyped(StubState *state, address_t address, const uint8_t *buffer, unsigned length)
{
    if (Memory_is_valid_address(state, address))
    {
        unsigned i;
        for (i = 0; i < length; i++)
            ((uint8_t *) address)[i] = buffer[i];
    }
}

/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include <stdint.h>

#include "RegisterMap.h"
#include "ArmRegisterMap.h"
#include "Assert.h"

register_t RegisterMap_get_register(StubState *state, unsigned reg)
{
    assert(reg <= REG_PC);

    return ((uint32_t *) state->register_map)[reg];
}

/**
 * Set a value in the register map.
 */
void RegisterMap_set_register(StubState *state, unsigned reg, register_t val)
{
    assert(reg <= REG_PC);

    ((uint32_t *) state->register_map)[reg] = val;
}

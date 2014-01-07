/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _REGISTER_MAP_H
#define _REGISTER_MAP_H

#include "StubState.h"

//Register number that is used to denote an invalid register
#define INVALID_REGISTER 0xffff

typedef uint32_t register_t; //For now this declaration is good enough

/**
 * Get a value from the register map.
 */
register_t RegisterMap_get_register(StubState *state, unsigned reg);

/**
 * Set a value in the register map.
 */
void RegisterMap_set_register(StubState *state, unsigned reg, register_t val);


#endif /* _REGISTER_MAP_H */

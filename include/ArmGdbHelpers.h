/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _ARM_GDB_HELPERS_H
#define _ARM_GDB_HELPERS_H

#include "StubState.h"

unsigned Gdb_map_gdb_register_number_to_stub(StubState *state, unsigned reg);
void Gdb_continue_execution(StubState *state);

#endif /* _ARM_GDB_HELPERS_H */

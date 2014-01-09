/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "StubState.h"
#include "HostInterface.h"


void stub_handle_exception(unsigned signal, void * register_map)
{
	StubState state;
    state.signal = signal;
    state.register_map = register_map;
    
    HostInterface_communicate(&state);
}

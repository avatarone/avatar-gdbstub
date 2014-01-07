/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

/**
 * This file specifies the prototype of the interface that the 
 * stub uses to communicate with the host.
 * Most is implementation-specific.
 */

#ifndef _HOST_INTERFACE_H
#define _HOST_INTERFACE_H

#include "StubState.h"

/**
 * Called one time during stub initialization.
 */
void HostInterface_init(StubState *state);

/**
 * Called by the stub when an event occured that
 * calls for communication with the host, e.g. a breakpoint has been hit.
 */
void HostInterface_communicate(StubState *state);



#endif /* _HOST_INTERFACE_H */

/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

/**
 * This file contains the general state structure of the stub.
 */
#ifndef _STUB_STATE_H
#define _STUB_STATE_H

/**
 * This structure encapsulates all data touched by the stub.
 */
typedef struct
{
    RegisterMap *register_map;
    HostInterfaceState host_interface;
    unsigned signal;
} StubState;

#endif /* _STUB_STATE_H */

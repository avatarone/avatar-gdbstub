/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _GDB_HOST_INTERFACE_H
#define _GDB_HOST_INTERFACE_H

#include <stdint.h>

/**
 * This structure encapsulates data for the GDB serial protocol host interface.
 */
//TODO: This is GDB specific, move it
typedef struct
{
    uint8_t input_checksum;
    uint8_t input_packet_finished;
    uint8_t output_checksum;
} HostInterfaceState;

#endif /* _GDB_HOST_INTERFACE_H */

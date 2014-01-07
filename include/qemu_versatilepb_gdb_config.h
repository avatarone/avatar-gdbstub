/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _QEMU_VERSATILEPB_GDB_CONFIG_H
#define _QEMU_VERSATILEPB_GDB_CONFIG_H

#define UART_PL011_BASE ((volatile uint32_t *) 0x101F1000)

#include "GdbHostInterface.h"
#include "ArmRegisterMap.h"

#endif /* _QEMU_VERSATILEPB_GDB_CONFIG_H */

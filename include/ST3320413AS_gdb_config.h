/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _ST3320413AS_GDB_CONFIG_H
#define _ST3320413AS_GDB_CONFIG_H

#define UART_16550_BASE ((volatile uint32_t *) 0x400D3000)

#define CONFIG_MEMORY_ERROR

#include "GdbHostInterface.h"
#include "ArmRegisterMap.h"

#endif /* _ST3320413AS_GDB_CONFIG_H */

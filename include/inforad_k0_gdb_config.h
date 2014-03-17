/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _INFORAD_K0_GDB_CONFIG_H
#define _INFORAD_K0_GDB_CONFIG_H

#define UART_SIRF_BASE ((volatile uint16_t *) 0x80030000)

#include "GdbHostInterface.h"
#include "ArmRegisterMap.h"
/* This include has to be last, to not trespass on definitions that should be made in other files */
#include "DefaultDefines.h"

#endif /* _INFORAD_K0_GDB_CONFIG_H */

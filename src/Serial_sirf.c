/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

/**
 * A driver for the uart found in the Inforad K0 SIRF chip.
 */

#include "Serial.h"


#ifndef UART_SIRF_BASE
#error Configuration value UART_SIRF_BASE must be set, e.g. to ((volatile uint16_t *) 0x80030000)
#endif 

#define UART_BASE (UART_SIRF_BASE)


#define UART_REG_RXDATA 3
#define UART_REG_TXDATA 2
#define UART_REG_FLAG 1

#define UART_FLAG_TXFF (1 << 6)
#define UART_FLAG_RXFF (1 << 4)

void Serial_init(void)
{
	/* Initialization is done by the inforad k0 bootloader */
}

int Serial_write_byte(uint8_t data)
{
    
    while (UART_BASE[UART_REG_FLAG] & UART_FLAG_TXFF)
		WATCHDOG_EXCITE;
    
    UART_BASE[UART_REG_TXDATA] = data;
    
    return 0;
}

int Serial_is_data_available(void)
{
    return (UART_BASE[UART_REG_FLAG] & UART_FLAG_RXFF) != 0;
}

int Serial_read_byte_blocking(void)
{
	int ret;
    while (!Serial_is_data_available())
		WATCHDOG_EXCITE;
    
	ret = UART_BASE[UART_REG_RXDATA];
	
	return ret;
}

void Serial_flush_write()
{
    while (UART_BASE[UART_REG_FLAG] & UART_FLAG_TXFF)
		WATCHDOG_EXCITE;
}

/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "Serial.h"

#ifndef UART_16550_BASE
#error Configuration value UART_16550_BASE must be set, e.g. to ((volatile uint32_t *) 0x400D3000)
#endif

#define UART_BASE UART_16550_BASE

#define UART_REG_DATA 0
#define UART_REG_LINE_STATUS 5

#define UART_LINE_FLAG_DATA_READY (1 << 0)
#define UART_LINE_FLAG_PARITY_ERROR (1 << 2)
#define UART_LINE_FLAG_OVERRUN_ERROR (1 << 1)
#define UART_LINE_FLAG_FRAMING_ERROR (1 << 3)
#define UART_LINE_FLAG_TRANSMITTER_HOLDING_REGISTER_EMPTY (1 << 5)
#define UART_LINE_FLAG_TRANSMITTER_EMPTY (1 << 5)

void Serial_init(void)
{
}

int Serial_write_byte(uint8_t data)
{
    while (!(UART_BASE[UART_REG_LINE_STATUS] & UART_LINE_FLAG_TRANSMITTER_HOLDING_REGISTER_EMPTY));
    
    UART_BASE[UART_REG_DATA] = (uint32_t) data;
    
    return 0;
}

int Serial_is_data_available(void)
{
    return (UART_BASE[UART_REG_LINE_STATUS] & UART_LINE_FLAG_DATA_READY) != 0;
}

int Serial_read_byte_blocking(void)
{
    while (!Serial_is_data_available());
    
    if (UART_BASE[UART_REG_LINE_STATUS] & UART_LINE_FLAG_PARITY_ERROR)
    {
        //TODO: some error
        return -1;
    }
    else if (UART_BASE[UART_REG_LINE_STATUS] & UART_LINE_FLAG_OVERRUN_ERROR)
    {
        //TODO: some error
        return -1;
    }
    else if (UART_BASE[UART_REG_LINE_STATUS] & UART_LINE_FLAG_FRAMING_ERROR)
    {
        //TODO: some error
        return -1;
    }
    else
    {
        return UART_BASE[UART_REG_DATA];
    }
    
    return -1;
}

void Serial_flush_write()
{
   while ((UART_BASE[UART_REG_LINE_STATUS] & UART_LINE_FLAG_TRANSMITTER_EMPTY) == 0); 
}

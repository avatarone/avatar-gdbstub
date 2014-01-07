/*
 *  Created on: Apr 20, 2013
 *
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include  "crc.h"

static unsigned char r1[16] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
};

static unsigned char r2[16] = {
    0x00, 0x9d, 0x23, 0xbe, 0x46, 0xdb, 0x65, 0xf8,
    0x8c, 0x11, 0xaf, 0x32, 0xca, 0x57, 0xe9, 0x74
};

/**
 * Calculate CRC8 of the data.
 * The polynom used is the same as for Dallas iButton products.
 */
unsigned char crc_calc(uint8_t * crc, uint8_t data)
{
    int i = (data ^ *crc) & 0xff;

    *crc = r1[i&0xf] ^ r2[i>>4];
    return *crc;
}

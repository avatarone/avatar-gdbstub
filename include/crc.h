/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

unsigned char crc_calc(uint8_t * crc, uint8_t data);

#endif /* CRC_H_ */

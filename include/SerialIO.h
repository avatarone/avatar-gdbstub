/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */
/* This header is used mostly for debugging to access the IO functions
 * of the serial port
 */
#ifndef _SERIALIO_H
#define _SERIALIO_H 1

void print_uint8(uint8_t val);
void print_uint32(uint32_t val);
void print_char(char c);

#endif

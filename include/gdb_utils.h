/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _GDB_UTILS_H
#define _GDB_UTILS_H

#include <stdint.h>
typedef unsigned size_t;

/* Defined in utils.c */
char nibble_to_hex_char(uint8_t nibble);
size_t strlen(const char * str);
int hex_char_to_nibble(char c);
int is_name_character(char c);

#endif /* _GDB_UTILS_H */

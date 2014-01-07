/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "gdb_utils.h"
    
char nibble_to_hex_char(uint8_t nibble)
{
    nibble &= 0xf;
    if (nibble < 10)
        return '0' + nibble;
    else
        return 'a' + nibble - 10;
}

/**
 * Get the length of an 8-bit string.
 */
size_t strlen(const char * str)
{
    size_t i;
    
    for (i = 0; str[i]; i++);
    return i;
}

int hex_char_to_nibble(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else return -1;
}

int is_name_character(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else
        return 0;
}

/**
 * Set a memory range to a value.
 * Only the lowest byte of 'c' is taken into account, the value is set byte-wise.
 */
void * memset(void * mem, int c, size_t len)
{
    size_t i = 0;
    
    for (i = 0; i < len; i++)
        ((uint8_t *) mem)[i] = (uint8_t) c;
        
    return mem;
}

/**
 * Compare to 8-bit strings, return 0 if they are equal, 1 otherwise. 
 */
int strcmp(const char * str1, const char * str2)
{
    int i;
    
    for (i = 0; str1[i] && str1[i] == str2[i]; i++);
    
    return !(str1[i] == str2[i]);
}

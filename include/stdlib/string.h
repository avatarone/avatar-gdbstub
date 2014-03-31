#ifndef _STDLIB_STRING_H
#define _STDLIB_STRING_H

#include "stdlib/stdlib.h"

void * memcpy(void *dst, const void *src, size_t len);
void * memset(void *buf, int c, size_t len);
size_t strlen(const char * str);
int strcmp(const char * str1, const char * str2);

#endif /* _STDLIB_STRING_H */

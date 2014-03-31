#include "stdlib/string.h"

void * memset(void *buf, int c, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
    {
        ((unsigned char *) buf)[i] = (unsigned char) c;
    }

    return buf;
}

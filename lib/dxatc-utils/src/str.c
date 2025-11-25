#include "dxatc-utils/str.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

char* dxAtcStrStr(char* haystack, const char* needle, int ignorecase)
{
    size_t haystacksz = strlen(haystack);
    size_t needlesz = strlen(needle);
    int match = 1;
    char* ret = NULL;

    char* needlebf = malloc(needlesz + 1);
    for(size_t i = 0; i < haystacksz; i++)
    {
        memset(needlebf, 0, needlesz + 1);
        memcpy(needlebf, haystack + i, needlesz);

        match = strcmp(needlebf, needle);
        if(ignorecase)
            match = strcasecmp(needlebf, needle);

        if(!match)
        {
            ret = haystack + i;
            break;
        }
    }
    free(needlebf);
    return ret;
}

size_t dxAtcStrApp(char* buffer, size_t buffersz, size_t* offset, const char* in, ...)
{
    va_list argp;
    size_t wrote = 0;
    size_t avail = buffersz - *offset;
    char* dest = buffer + *offset;

    if(*offset > buffersz - 2)
        return 0;

    /* TODO : Check for incorrect args */
    va_start(argp, in);
    wrote = vsnprintf(dest, avail, in, argp);
    va_end(argp);

    /*wrote = snprintf(dest, avail, "%s", in);*/
    /*printf("%zu %zu %zu %p\n", avail, wrote, *offset, dest);*/
    *offset = *offset + wrote;
    return wrote;

}

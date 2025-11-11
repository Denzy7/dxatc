#ifndef DXATC_UTILS_STR_H
#define DXATC_UTILS_STR_H
#include "dxatc-utils/macros.h"

#include <stddef.h>

DXATC_UTILS_MACROS_CDECLS_BEGIN

const char* dxAtcStrStr(const char* haystack, const char* needle, int ignorecase);

size_t dxAtcStrApp(char* buffer, size_t buffersz, size_t* offset, const char* in, ...);

DXATC_UTILS_MACROS_CDECLS_END

#endif

#ifndef DXATC_ENGINE_AIRPORT_XPLANE_H
#define DXATC_ENGINE_AIRPORT_XPLANE_H

#include "dxatc-engine/airport.h"
#include "dxatc-utils/macros.h"
#include <stddef.h>

DXATC_UTILS_MACROS_CDECLS_BEGIN

int dxAtcAirportDbXPlaneLoad(const char* xplanepath, DxAtcAirportDb* db);

DXATC_UTILS_MACROS_CDECLS_END

#endif

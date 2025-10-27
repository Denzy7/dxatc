#ifndef DXATC_ENGINE_METAR_XPLANE_H
#define DXATC_ENGINE_METAR_XPLANE_H

#include "dxatc-engine/metar.h"
#include "dxatc-engine/airport.h"
#include "dxatc-utils/macros.h"

DXATC_UTILS_MACROS_CDECLS_BEGIN

int dxAtcMetarXPlaneFetch(const char* ip, DxAtcMetar* metar, const DxAtcAirportDb* db);

DXATC_UTILS_MACROS_CDECLS_END
#endif

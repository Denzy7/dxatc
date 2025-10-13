#ifndef DXATC_ENGINE_METAR_H
#define DXATC_ENGINE_METAR_H
#include "dxatc-engine/weather.h"
#include "dxatc-engine/types.h"
#include "dxatc-utils/macros.h"

typedef enum 
{
    DXATC_ENGINE_METAR_TYPE_METAR,
    DXATC_ENGINE_METAR_TYPE_SPECI,
}DxAtcMetarType;

typedef struct
{
    DxAtcMetarType type;
    DxAtcAirportICAO icao;
    DxAtcZulu zulu;
    DxAtcWeatherWind wind;
    int visibility;
    DxAtcWeatherPrecip precip;
    DxAtcWeatherCloud** sky;
    int oat, dewpt;
    int qnh;
}DxAtcMetar;

DXATC_UTILS_MACROS_CDECLS_BEGIN

int dxAtcMetarInit(DxAtcMetar* metar);
void dxAtcMetarFree(DxAtcMetar* metar);

int dxAtcMetarParse(const char* string, DxAtcMetar* metar);

DXATC_UTILS_MACROS_CDECLS_END

#endif


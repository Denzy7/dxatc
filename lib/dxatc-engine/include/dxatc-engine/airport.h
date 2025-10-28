#ifndef DXATC_ENGINE_AIRPORT_H
#define DXATC_ENGINE_AIRPORT_H

#include "dxatc-engine/aircraft.h"
#include "dxatc-engine/airline.h"
#include "dxatc-engine/types.h"

#include "dxatc-utils/macros.h"

#include <stddef.h>

typedef enum
{
    DXATC_ENGINE_AIRPORT_RUNWAY_TYPE_LAND,
    /* DXATC_ENGINE_AIRPORT_RUNWAY_TYPE_SEA, */
    /* DXATC_ENGINE_AIRPORT_RUNWAY_TYPE_HELIPAD, */
}DxAtcRunwayType;

typedef struct{
    DxAtcLatLon latlon;
    DxAtcRunwayNumber number;
    float threshold;
    float blastpad;
}DxAtcAirportRunwayEnd;

typedef struct
{
    DxAtcRunwayType type;
    DxAtcAirportRunwayEnd ends[2];
}DxAtcAirportRunway;

typedef struct
{
    DxAtcLatLon latlon;
}DxAtcAirportTaxiwayPoint;

typedef struct
{
    DxAtcAirportTaxiwayPoint** points;
}DxAtcAirportTaxiwayLine;

typedef struct
{
    char* name;
    DxAtcAirportTaxiwayLine** lines;
}DxAtcAirportTaxiway;

typedef struct{
    DxAtcLatLon latlon;
    char* name;
    DxAtcAirline* airline;
    DxAtcAircraftOperations operations;
}DxAtcAirportStand;

typedef struct
{
    DxAtcLatLon latlon;
    DxAtcAirportICAO icao;
    char name[64];
    int elev_msl;
    int transition_alt;
    int transition_lvl;
    DxAtcAirportTaxiway** taxiways;
    DxAtcAirportRunway** runways;
    size_t runways_count;
}DxAtcAirport;

typedef struct
{
   DxAtcAirport** airports;
   size_t airports_count;
}DxAtcAirportDb;

DXATC_UTILS_MACROS_CDECLS_BEGIN

int dxAtcAirportDbFind(const DxAtcAirportICAO icao, DxAtcAirport** apt, const DxAtcAirportDb* db);
void dxAtcAirportDbFree(DxAtcAirportDb* db);

DXATC_UTILS_MACROS_CDECLS_END

#endif

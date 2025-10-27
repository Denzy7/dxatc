#ifndef DXATC_ENGINE_AIRPORT_H
#define DXATC_ENGINE_AIRPORT_H

#include "dxatc-engine/aircraft.h"
#include "dxatc-engine/airline.h"
#include "dxatc-engine/types.h"

#include "dxatc-utils/macros.h"

#include <stddef.h>

typedef struct
{
    DxAtcLatLon latlon_end1, latlon_end2;
    DxAtcRunwayNumber number_end1;
    DxAtcRunwayNumber number_end2;
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
    char name[32];
    int elev_msl;
    int transition_alt;
    int transition_lvl;
    DxAtcAirportTaxiway** taxiways;
    DxAtcAirportRunway** runways;
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

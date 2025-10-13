#ifndef DXATC_ENGINE_AIRPORT_H
#define DXATC_ENGINE_AIRPORT_H

#include "dxatc-engine/aircraft.h"
#include "dxatc-engine/airline.h"
#include "dxatc-engine/types.h"

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
    int elev_msl;
    int transition_alt;
    int transition_lvl;
    DxAtcAirportTaxiway** taxiways;
    DxAtcAirportRunway** runways;
}DxAtcAirport;

#endif

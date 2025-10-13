#ifndef DXATC_ENGINE_AIRCRAFT_H
#define DXATC_ENGINE_AIRCRAFT_H

#include "dxatc-engine/frequency.h"
#include "dxatc-engine/callsign.h"
#include "dxatc-engine/flightplan.h"
#include "dxatc-engine/transponder.h"

typedef enum 
{
    DXATC_ENGINE_AIRCRAFT_PHASE_PARKED,
    DXATC_ENGINE_AIRCRAFT_PHASE_TAXI,
    DXATC_ENGINE_AIRCRAFT_PHASE_TAKEOFF,
    DXATC_ENGINE_AIRCRAFT_PHASE_CLIMB,
    DXATC_ENGINE_AIRCRAFT_PHASE_CRUISE,
    DXATC_ENGINE_AIRCRAFT_PHASE_DESCEND,
    DXATC_ENGINE_AIRCRAFT_PHASE_APPROACH,
    DXATC_ENGINE_AIRCRAFT_PHASE_LANDING,

    DXATC_ENGINE_AIRCRAFT_PHASE_COUNT
}DxAtcAircraftPhase;

typedef enum{
    DXATC_ENGINE_AIRCRAFT_CATEGORY_AIRPLANE,
    /* DXATC_ENGINE_AIRCRAFT_CATEGORY_ROTORCRAFT, */

    DXATC_ENGINE_AIRCRAFT_CATEGORY_COUNT,
}DxAtcAircraftCategory;

typedef enum
{
    DXATC_ENGINE_AIRCRAFT_RUNWAY_SEA = 0,
    DXATC_ENGINE_AIRCRAFT_RUNWAY_LAND = 1,

    DXATC_ENGINE_AIRCRAFT_RUNWAY_COUNT
}DxAtcAircraftRunway;

typedef enum
{
    DXATC_ENGINE_AIRCRAFT_OPERATIONS_NONE = 0,
    DXATC_ENGINE_AIRCRAFT_OPERATIONS_GENERALAVIATION = 1,
    DXATC_ENGINE_AIRCRAFT_OPERATIONS_AIRLINE = 2,
    DXATC_ENGINE_AIRCRAFT_OPERATIONS_CARGO = 1 << 2,
    /* DXATC_ENGINE_AIRCRAFT_OPERATIONS_MILITARY = 1 << 4, */

    DXATC_ENGINE_AIRCRAFT_OPERATIONS_ALL = 0x7fffffff,
}DxAtcAircraftOperations;

typedef struct
{
    DxAtcAircraftPhase phase;
    DxAtcAircraftCategory category;
    DxAtcAircraftRunway runway;
    DxAtcAircraftOperations operations;
    DxAtcTransponder transponder;
    DxAtcCallsign callsign;
    DxAtcFrequency com1, com2;
    DxAtcFlightPlan flightplan;
}DxAtcAircraft;

#endif


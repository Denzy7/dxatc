#ifndef DXATC_ENGINE_FLIGHTPLAN_H
#define DXATC_ENGINE_FLIGHTPLAN_H

#include "dxatc-engine/navdata.h"
typedef enum
{
    DXATC_ENGINE_FLIGHTPLAN_RULES_VFR,
    DXATC_ENGINE_FLIGHTPLAN_RULES_IFR,

    DXATC_ENGINE_FLIGHTPLAN_RULES_COUNT
}DxAtcFlightPlanRules;

typedef struct
{
    DxAtcAirportICAO dept_icao;
    DxAtcAirportICAO arrv_icao[5];
    DxAtcFlightPlanRules rules;
    DxAtcNavdataProcedure* sid;
    DxAtcNavdataProcedure* star;
    DxAtcNavdataProcedure* approach;
    DxAtcNavdataNavaid** waypoints;
    int cruise_alt;
    int cruise_tas;
}DxAtcFlightPlan;

#endif

#ifndef DXATC_ENGINE_CALLSIGN_H
#define DXATC_ENGINE_CALLSIGN_H

#include "dxatc-engine/airline.h"

typedef struct
{
    char reg[6];
    DxAtcAirline* airline;
    char* flight;
}DxAtcCallsign;

#endif

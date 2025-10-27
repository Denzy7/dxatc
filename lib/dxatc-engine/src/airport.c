#include "dxatc-engine/airport.h"

#include <stdlib.h>
#include <string.h>

int dxAtcAirportDbFind(const DxAtcAirportICAO icao, DxAtcAirport** apt, const DxAtcAirportDb* db)
{
    for(size_t i = 0; i < db->airports_count; i++)
    {
        if(strstr(icao, db->airports[i]->icao))
        {
            *apt = db->airports[i];
            return 1;
        }
    }

    return 0;
}

void dxAtcAirportDbFree(DxAtcAirportDb* db)
{
    for(size_t i = 0; i < db->airports_count; i++)
    {
        if(db->airports[i])
            free(db->airports[i]);
        else
            break;
    }
    free(db->airports);
}

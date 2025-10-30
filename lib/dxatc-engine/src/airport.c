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
    DxAtcAirport* apt = NULL;
    DxAtcAirportRunway* rwy = NULL;
    DxAtcController* con = NULL;
    for(size_t i = 0; i < db->airports_count; i++)
    {
        apt = db->airports[i];
        if(apt){
            for(size_t j = 0; j < apt->runways_count; j++)
            {
                rwy = apt->runways[j];
                if(rwy)
                    free(rwy);
            }
            for(size_t j = 0; j < apt->controllers_count; j++)
            {
                con = apt->controllers[j];
                if(con)
                    free(con);
            }
            free(apt->controllers);
            free(apt->runways);
            free(apt);
        }
        else{
            break;
        }
    }
    free(db->airports);
}

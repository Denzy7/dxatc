#include "dxatc-engine/airport.h"
#include "dxatc-utils/str.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int dxAtcAirportDbFind(const DxAtcAirportICAO icao, DxAtcAirport** apt, const DxAtcAirportDb* db)
{
    DxAtcAirportICAO myicao;

    memset(myicao, 0, sizeof(myicao));
    snprintf(myicao, sizeof(myicao), "%s", icao);
    for(size_t i = 0; i < db->airports_count; i++)
    {
        if(dxAtcStrStr(myicao, db->airports[i]->icao, 1))
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

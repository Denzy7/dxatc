#include "dxatc-engine/airport_xplane.h"
#include "dxatc-utils/vvtor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int dxAtcAirportDbXPlaneLoad(const char* xplanepath, DxAtcAirportDb* db)
{
    char defaultaptdatpath[1024];
    FILE* defaultaptdat;
    char linebf[2048];
    DxAtcAirport apt, *aptcpy;
    DxAtcAirportRunway *rwycpy;
    /*char aptfmtstr[32];*/
    char xp1302_label[33], xp1302_data[65];
    int objid = 0;
    int firstapt = 1;
    uint32_t lineno = 0;
    DxAtcAirport*** airports = NULL;
    size_t* count = NULL;

    if(xplanepath == NULL)
    {
        fprintf(stderr, "must provide xplane path to locate xplane airport files!\n");
        return 0;
    }

    if(db == NULL)
    {
        fprintf(stderr, "must provide a db object to put airport in!\n");
        return 0;
    }

    airports = &db->airports;
    count = &db->airports_count;

    snprintf(defaultaptdatpath, sizeof(defaultaptdatpath),
            "%s/Resources/default scenery/default apt dat/Earth nav data/apt.dat",
            xplanepath);

    if((defaultaptdat = fopen(defaultaptdatpath, "rb")) == NULL)
    {
        fprintf(stderr, "failed to open default apt.dat specified in %s\n", defaultaptdatpath);
        return 0;
    }

    vvtor_init((void***)airports);

    while(fgets(linebf, sizeof(linebf), defaultaptdat))
    {
        sscanf(linebf, "%d", &objid);

        if((objid == 1 && !firstapt) || objid == 99){

            if(apt.latlon[0] == 0 && apt.latlon[1] == 0 && apt.runways[0])
            {
                memcpy(apt.latlon, apt.runways[0]->ends[0].latlon, sizeof(apt.latlon));
            }

            aptcpy = malloc(sizeof(DxAtcAirport));
            memcpy(aptcpy, &apt, sizeof(DxAtcAirport));
            vvtor_push((void***)airports, aptcpy);

            if(count)
                *count = *count + 1;
        }

        if(objid == 1 && lineno > 0){
            memset(&apt, 0, sizeof(apt));

            sscanf(linebf, "%d%d%*d%*d%32s %63c", &objid, &apt.elev_msl, apt.icao, apt.name);
            apt.name[strlen(apt.name) - 1] = 0;
            firstapt = 0;

            vvtor_init((void***)&apt.runways);
        }
        if(objid == 1302)
        {
            sscanf(linebf, "%*d%32s%64s", xp1302_label, xp1302_data);
            if(!strcmp(xp1302_label, "datum_lat"))
                sscanf(xp1302_data, "%f", &apt.latlon[0]);
            if(!strcmp(xp1302_label, "datum_lon"))
                sscanf(xp1302_data, "%f", &apt.latlon[1]);
            if(!strcmp(xp1302_label, "transition_alt"))
                sscanf(xp1302_data, "%d", &apt.transition_alt);
            if(!strcmp(xp1302_label, "transition_level"))
                sscanf(xp1302_data, "%d", &apt.transition_lvl);
        }

        if(objid == 100)
        {
            rwycpy = malloc(sizeof(DxAtcAirportRunway));
            /*rwycpy->type = DXATC_ENGINE_AIRPORT_RUNWAY_TYPE_LAND;*/
            sscanf(linebf,
                    "%*d%*f%*d%*d%*f%*d%*d%*d"

                    "%s%f%f%f%f%*d%*d%*d%*d"
                    "%s%f%f%f%f%*d%*d%*d%*d"
                    ,

                    rwycpy->ends[0].number,
                    &rwycpy->ends[0].latlon[0],
                    &rwycpy->ends[0].latlon[1],
                    &rwycpy->ends[0].threshold,
                    &rwycpy->ends[0].blastpad,

                    rwycpy->ends[1].number,
                    &rwycpy->ends[1].latlon[0],
                    &rwycpy->ends[1].latlon[1],
                    &rwycpy->ends[1].threshold,
                    &rwycpy->ends[1].blastpad
                  );
            vvtor_push((void***)&apt.runways, rwycpy);
            apt.runways_count++;
        }

        lineno++;
    }


    fclose(defaultaptdat);

    /* TODO: override with custom scenery */

    return 1;
}

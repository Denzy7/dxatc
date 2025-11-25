#include "dxatc-engine/airport_xplane.h"
#include "dxatc-utils/vvtor.h"
#include "dxatc-utils/str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>


#define _DXATC_ENGINE_AIRPORT_XPLANE_ATCNAME_MAXNAMES 1 + 4 /* must have atleast 1  + extra names*/
#define _DXATC_ENGINE_AIRPORT_XPLANE_ATCNAME_MAXKWDS 1 + 4 /* 1 for the actual name + extra kwds */
static const char* const atcname_type
[DXATC_ENGINE_CONTROLLER_TYPE_COUNT]
[_DXATC_ENGINE_AIRPORT_XPLANE_ATCNAME_MAXNAMES]
[_DXATC_ENGINE_AIRPORT_XPLANE_ATCNAME_MAXKWDS] = {
    {
        /* atis, awos,... */
        {"ATIS"},
        {"AWOS"},
        /*{"ATIS Departure", "ATIS Dep"},*/
        /*{"ATIS Arrival", "ATIS Arr"}*/
    },
    {
        /* ctaf, unicom ... */
        {"Traffic", " CTAF"},
        {"Unicom", " UNICOM"},
    },
    {
        /* clearance */
        {"Clearance", "CLNC"},
    },
    {
        /* ground, apron */
        {"Ground", "GND", "Ground", "TAXIING", "-Ground"},
        {"Apron", "APRON", "-Apron"},
    },
    {
        /* tower */
        {"Tower", "TWR", "Tower", "-Tower"},
    },
    {
        /* app, director, radar */
        {"Approach", "APP", "Approach", "APP/DEP"},
        {"Radar", "RADAR", "RDR"},
        {"Director", "Director"},
    },
    {
        /* dep, radar */
        {"Departure", "DEP", "Departure", "APP/DEP"},
        {"Control", "Control"},
        {"Radar", " RADAR", " RDR"},
    },
    {
        /* center, flight services */
        {"Center"},
    },
};

char* _dxAtcAirportDbXPlaneAtcLocation(char* atcname, DxAtcControllerType type, int* i)
{
    char* findstr = NULL;
    const char* type_name = NULL;
    int requestbreak = 0;
    *i = _DXATC_ENGINE_AIRPORT_XPLANE_ATCNAME_MAXNAMES - 1;
    while(1)
    {
        for(size_t j = 1; j < _DXATC_ENGINE_AIRPORT_XPLANE_ATCNAME_MAXKWDS; j++)
        {
            type_name = atcname_type[type][*i][j];
            if(type_name && (findstr = dxAtcStrStr(atcname, type_name, 1))){
                requestbreak = 1;
                break;
            }
        }
        if(*i == 0 || requestbreak)
            break;
        (*i)--;
    }
    return findstr;
}

int dxAtcAirportDbXPlaneLoad(const char* xplanepath, DxAtcAirportDb* db)
{
    char defaultaptdatpath[1024];
    FILE* defaultaptdat;
    char linebf[2048];
    DxAtcAirport apt, *aptcpy;
    DxAtcAirportRunway *rwycpy;
    DxAtcController* atccpy;
    /*char aptfmtstr[32];*/
    char xp1302_label[33], xp1302_data[65];
    int objid = 0;
    int firstapt = 1;
    uint32_t lineno = 0;

    DxAtcAirport*** airports = NULL;
    size_t* count = NULL;

    char* findstr = NULL;
    /* a subtle way to tell compiler to stfu with truncation warnings
     * we are tho truncating to a smaller buffer.
     * have you ever heard of an ATC callsign longer than 63 characters tho???
     * */
    char*speci_atc_name;
    char controllername_location[64], controllername_type[64];
    int ii_atcname;

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
            /* some post processing */

            if(apt.latlon[0] == 0 && apt.latlon[1] == 0 && apt.runways[0])
            {
                memcpy(apt.latlon, apt.runways[0]->ends[0].latlon, sizeof(apt.latlon));
            }
            for(size_t i = 0; i < apt.controllers_count; i++)
            {
                atccpy = apt.controllers[i];
                memset(controllername_location, 0, sizeof(controllername_location));
                memset(controllername_type, 0, sizeof(controllername_type));
                speci_atc_name = NULL;

                findstr = _dxAtcAirportDbXPlaneAtcLocation(atccpy->name, atccpy->type, &ii_atcname);

                if(findstr)
                {
                    *findstr = 0;
                    if(*(findstr - 1) == ' ')
                        *(findstr - 1) = 0;
                    speci_atc_name = strdup(atccpy->name);
                    findstr = speci_atc_name;
                }else {
                    findstr = "";
                }

                snprintf(controllername_location, sizeof(controllername_location),
                        "%s%s", findstr,
                        atccpy->type == DXATC_ENGINE_CONTROLLER_TYPE_CTAF && !strlen(findstr) ?
                            apt.name : "");
                snprintf(controllername_type, sizeof(controllername_type), "%s", atcname_type[atccpy->type][ii_atcname][0]);
                if(speci_atc_name)
                    free(speci_atc_name);

                snprintf(atccpy->name, sizeof(atccpy->name), "%s%s%s",
                        controllername_location,
                        strlen(controllername_location) ? " " : "",
                        controllername_type);

                if(atccpy->type > DXATC_ENGINE_CONTROLLER_TYPE_ATIS){

                    atccpy->name[0] = toupper(atccpy->name[0]);
                    for(size_t j = 1; j < strlen(atccpy->name); j++)
                    {
                        if(atccpy->name[j - 1] != ' ')
                            atccpy->name[j] = tolower(atccpy->name[j]);
                        else
                            atccpy->name[j] = toupper(atccpy->name[j]);
                    }
                }
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
            vvtor_init((void***)&apt.controllers);
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
            rwycpy = calloc(1, sizeof(DxAtcAirportRunway));
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


        if((objid >= 50 && objid <= 56) || (objid >= 1050 && objid <= 1056))
        {
            atccpy = calloc(1, sizeof(DxAtcController));
            if(objid >= 1050)
                atccpy->type = objid - 1050;
            if(objid <= 56)
                atccpy->type = objid - 50;
            sscanf(linebf, "%*d%6s %31c", atccpy->freq, atccpy->name);

            vvtor_push((void***)&apt.controllers, atccpy);
            apt.controllers_count++;
        }

        /* not resetting objid causes the previous
         * objid to be parsed if an empty line is read by fgets
         */
        objid = 0;
        lineno++;
    }


    fclose(defaultaptdat);

    /* TODO: override with custom scenery */

    return 1;
}

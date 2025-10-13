#ifndef DXATC_ENGINE_NAVDATA_H
#define DXATC_ENGINE_NAVDATA_H

#include "dxatc-engine/types.h"

typedef enum
{
    DXATC_ENGINE_NAVDATA_NAVAID_TYPE_RNAV,
    DXATC_ENGINE_NAVDATA_NAVAID_TYPE_VOR,
    DXATC_ENGINE_NAVDATA_NAVAID_TYPE_NDB,
    
    DXATC_ENGINE_NAVDATA_NAVAID_TYPE_COUNT
}DxAtcNavdataNavaidType;

typedef enum
{
    DXATC_ENGINE_NAVDATA_PROCEDURE_SID,
    DXATC_ENGINE_NAVDATA_PROCEDURE_STAR,
    DXATC_ENGINE_NAVDATA_PROCEDURE_APPROACH,

    DXATC_ENGINE_NAVDATA_PROCEDURE_COUNT
}DxAtcNavdataProcedureType;

typedef enum{
    DXATC_ENGINE_NAVDATA_APPROACH_TYPE_VISUAL,
    DXATC_ENGINE_NAVDATA_APPROACH_TYPE_ILS,
    DXATC_ENGINE_NAVDATA_APPROACH_TYPE_RNAV,
    /* DXATC_ENGINE_NAVDATA_APPROACH_TYPE_GLS, */
    DXATC_ENGINE_NAVDATA_APPROACH_TYPE_VORDME,

    DXATC_ENGINE_NAVDATA_APPROACH_TYPE_COUNT
}DxAtcNavdataApproachType;

typedef struct
{
    DxAtcNavdataNavaidType type;
    DxAtcLatLon latlon;
    DxAtcNavaidIdent ident;
    DxAtcRegionICAOPrefix icao_prefix;
    char* name;
}DxAtcNavdataNavaid;

typedef struct{
    DxAtcNavdataProcedureType type;
    DxAtcAirportICAO apt_icao;
    DxAtcProcedureIdent ident;
    DxAtcRunwayNumber rwy;
    DxAtcNavdataNavaid** waypoints;
    struct DxAtcNavdataProcedure** transitions;
}DxAtcNavdataProcedure;

typedef struct
{
    int cycle;    
    DxAtcNavdataNavaid** navaids;
    DxAtcNavdataProcedure** procedures;
}DxAtcNavdata;

#endif

#ifndef DXATC_ENGINE_TRANSPONDER_H
#define DXATC_ENGINE_TRANSPONDER_H

#include "dxatc-engine/types.h"

typedef enum
{
    DXATC_ENGINE_TRANSPONDER_OFF = 0,
    DXATC_ENGINE_TRANSPONDER_MODE_A = 1,
    DXATC_ENGINE_TRANSPONDER_MODE_C = 2,
    DXATC_ENGINE_TRANSPONDER_MODE_S = 1 << 2
}DxAtcTransponderMode;

typedef enum
{
    DXATC_ENGINE_TRANSPONDER_NAVMODE_NONE = 0,
    DXATC_ENGINE_TRANSPONDER_NAVMODE_AUTOPILOT = 1,
    DXATC_ENGINE_TRANSPONDER_NAVMODE_TCAS = 2,
    DXATC_ENGINE_TRANSPONDER_NAVMODE_LNAV = 1 << 2,
    DXATC_ENGINE_TRANSPONDER_NAVMODE_VNAV = 1 << 3,
    DXATC_ENGINE_TRANSPONDER_NAVMODE_ALTHOLD = 1 << 4,
    DXATC_ENGINE_TRANSPONDER_NAVMODE_APPR = 1 << 5,
}DxAtcTransponderNavMode;

typedef struct
{
    DxAtcTransponderMode mode;

    /* mode a*/
    int ident;
    DxAtcTransponderCode code;
    DxAtcLatLon latlon_radar;

    /* mode c */
    int alt_pressure;

    /* mode s */
    DxAtcTransponderNavMode navmode;
    DxAtcLatLon latlon_gps;
    int ias, gs;
    float mach;
    int alt_gps;
    int hdg_mag;
    int ap_hdg;
    int ap_alt;
}DxAtcTransponder;

#endif

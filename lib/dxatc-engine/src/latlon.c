#include "dxatc-engine/latlon.h"

#include <math.h>

/* https://en.wikipedia.org/wiki/Haversine_formula */
#define _DXATC_LATLON_EARTHRADIUS_NMI 3440.172786177106f
float _dxAtcLatLonAHavSin(float havsin)
{
    return acos(1.0f - (2.0f * havsin));
}
float _dxAtcLatLonHavSin(float radian)
{
    return (1.0f - cos(radian)) / 2.0f;
}
float dxAtcLatLonDistanceNMI(DxAtcLatLon a, DxAtcLatLon b)
{
    float phi1 = a[0] * DXATC_UTILS_MACROS_DEG_TO_RAD;
    float phi2 = b[0] * DXATC_UTILS_MACROS_DEG_TO_RAD;
    float dphi = phi2 - phi1;
    float dlam = (b[1] - a[1]) * DXATC_UTILS_MACROS_DEG_TO_RAD;
    float havsin = _dxAtcLatLonHavSin(dphi) + (cos(phi1) * cos(phi2) * _dxAtcLatLonHavSin(dlam));
    float theta = _dxAtcLatLonAHavSin(havsin);

    return _DXATC_LATLON_EARTHRADIUS_NMI * theta;
}

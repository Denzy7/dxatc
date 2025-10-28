#include "dxatc-engine/latlon.h"

#include <math.h>

float dxAtcLatLonDistanceNMI(DxAtcLatLon a, DxAtcLatLon b)
{
    float difx = b[0] - a[0];
    float dify = b[1] - a[1];

    float powx = pow(difx, 2.0f);
    float powy = pow(dify, 2.0f);

    return 60.0f * sqrt(powx + powy);
}

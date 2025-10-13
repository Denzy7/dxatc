#ifndef DXATC_ENGINE_WORLD_H
#define DXATC_ENGINE_WORLD_H

#include "dxatc-engine/types.h"

typedef struct
{
    DxAtcLatLon latlon;
    int range;
}DxAtcWorldBounds;

typedef struct
{
    DxAtcWorldBounds bounds;
}DxAtcWorld;

#endif

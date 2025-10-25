#include <dxatc-engine/metar_xplane.h>
#include <stdio.h>
#include <string.h>

int main()
{
    DxAtcMetar metar;
    char decoded[2048];

    memset(&metar, 0, sizeof(metar));

    dxAtcMetarInit(&metar);
    dxAtcMetarXPlaneFetch(NULL, &metar);
    dxAtcMetarDecode(decoded, sizeof(decoded), 
            DXATC_ENGINE_METAR_DECODE_FLAG_ALTIMETER_INHG | DXATC_ENGINE_METAR_DECODE_FLAG_VISIBILITY_SM,
            &metar);

    printf("%s", decoded);

    dxAtcMetarFree(&metar);
}

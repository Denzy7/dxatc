#include <dxatc-engine/metar_xplane.h>
#include <dxatc-engine/airport_xplane.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    DxAtcMetar metar;
    char decoded[2048];
    DxAtcAirportDb db;

    if(argc < 2)
    {
        fprintf(stderr, "must pass xpdir in argv1\n");
        return 1;
    }

    memset(&metar, 0, sizeof(metar));
    memset(&db, 0, sizeof(db));

    printf("loading xplane db... ");
    fflush(stdout);
    if(!dxAtcAirportDbXPlaneLoad(argv[1], &db))
    {
        fprintf(stderr, "cannot load xplane airports in %s\n", argv[1]);
        return 1;
    }
    printf("loaded %u airports\n", (uint32_t)db.airports_count);

    dxAtcMetarInit(&metar);
    dxAtcMetarXPlaneFetch(NULL, &metar, &db);
    dxAtcMetarDecode(decoded, sizeof(decoded), 
            DXATC_ENGINE_METAR_DECODE_FLAG_ALTIMETER_INHG | DXATC_ENGINE_METAR_DECODE_FLAG_VISIBILITY_SM,
            &metar, &db);

    printf("%s", decoded);

    if(db.airports)
        dxAtcAirportDbFree(&db);
    dxAtcMetarFree(&metar);
}

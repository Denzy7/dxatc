#include <dxatc-engine/airport_xplane.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    DxAtcAirportDb db;
    size_t i;

    if(argc < 2)
    {
        fprintf(stderr, "use argv1 to provide xplane location");
        return 1;
    }

    memset(&db, 0, sizeof(db));
    if(!dxAtcAirportDbXPlaneLoad(argv[1], &db))
    {
        fprintf(stderr, "failed to load xplane db from %s\n", argv[1]);
        return 1;
    }

    i = 0;
    printf("showing first 25 airports\n");
    while(db.airports[i] && i < 25)
    {
        printf(
                "airport:%s\n"
                "\ticao:%s\n"
                "\tlatlon:[%f, %f]\n"
                "\ttrans_alt:%d\n"
                "\ttrans_lvl:%d\n"
                "\n"
                ,
                db.airports[i]->name,
                db.airports[i]->icao,
                db.airports[i]->latlon[0],
                db.airports[i]->latlon[1],
                db.airports[i]->transition_alt,
                db.airports[i]->transition_alt
              );
        i++;
    }

    printf("loaded %u airports in total\n", (uint32_t) db.airports_count);

    dxAtcAirportDbFree(&db);

    return 0;
}

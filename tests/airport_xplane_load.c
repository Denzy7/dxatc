#include <dxatc-engine/airport_xplane.h>
#include <dxatc-engine/latlon.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    DxAtcAirportDb db;
    size_t i;
    DxAtcAirportRunway* rwy;
    DxAtcAirportRunwayEnd* rwyend;
    float rwylen, lda;

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
                "airport:%s (%s)\n"
                "\tlatlon:[%f, %f]\n"
                "\ttrans_alt:%d\n"
                "\ttrans_lvl:%d\n"
                ,
                db.airports[i]->name,
                db.airports[i]->icao,
                db.airports[i]->latlon[0],
                db.airports[i]->latlon[1],
                db.airports[i]->transition_alt,
                db.airports[i]->transition_alt
              );
        printf("\trunways:\n");
        for(size_t j = 0; j < db.airports[i]->runways_count; j++)
        {
            rwy = db.airports[i]->runways[j];
            rwylen = dxAtcLatLonDistanceNMI(rwy->ends[0].latlon, rwy->ends[1].latlon);

            printf("\t\t%s/%s\n", rwy->ends[0].number, rwy->ends[1].number);

            for(size_t k = 0; k < 2; k++)
            {
                rwyend = &rwy->ends[k];
                lda = ((rwylen * DXATC_UTILS_MACROS_NMI_TO_M) - rwyend->blastpad - rwyend->threshold) * DXATC_UTILS_MACROS_M_TO_FT;
                printf("\t\t\t%s latlon:[%f, %f] lda:%.0fft\n",
                        rwyend->number, rwyend->latlon[0], rwyend->latlon[1], lda);
            }

        }

        printf("\n");
        i++;
    }

    printf("loaded %u airports in total\n", (uint32_t) db.airports_count);

    dxAtcAirportDbFree(&db);

    return 0;
}

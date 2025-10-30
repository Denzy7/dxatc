#include <dxatc-engine/airport_xplane.h>
#include <dxatc-engine/latlon.h>
#include <dxatc-utils/macros.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void dumpapt(DxAtcAirport* apt)
{
    DxAtcController* con;
    DxAtcFrequencyFormatted freq;
    DxAtcAirportRunway* rwy;
    DxAtcAirportRunwayEnd* rwyend;
    float rwylen, lda;

    printf(
            "airport:%s (%s)\n"
            "\tlatlon:[%f, %f]\n"
            "\ttrans_alt:%d\n"
            "\ttrans_lvl:%d\n"
            ,
            apt->name,
            apt->icao,
            apt->latlon[0],
            apt->latlon[1],
            apt->transition_alt,
            apt->transition_alt
          );
    printf("\trunways:\n");
    for(size_t j = 0; j < apt->runways_count; j++)
    {
        rwy = apt->runways[j];
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
    if(apt->controllers_count)
    {
        memset(freq, 0, sizeof(freq));
        printf("\tcomms:\n");
        for(size_t j = 0; j < apt->controllers_count; j++)
        {
            con = apt->controllers[j];
            dxAtcFrequencyFormat(freq, con->freq);
            printf("\t\t%s %s\n", con->name, freq);
        }
    }

    printf("\n");
}


static const char* interesting[] =
{
    "KJFK",
    "KSEA",
    "KSFO",
    "KLAX",
    "HKJK",
    "DNMM",
    "FACT",
    "EGLL",
    "EHAM",
    "EDDF",
    "RJTT",
    "VHHH",
    "VABB",

    "H04",
    "S50",
};
int main(int argc, char* argv[])
{
    DxAtcAirportDb db;
    DxAtcAirport* apt;
    size_t i;

    if(argc < 2)
    {
        fprintf(stderr, "use argv1 to provide xplane location");
        return 1;
    }

    memset(&db, 0, sizeof(db));
    printf("loading xpdb... ");
    fflush(stdout);
    if(!dxAtcAirportDbXPlaneLoad(argv[1], &db))
    {
        fprintf(stderr, "failed to load xplane db from %s\n", argv[1]);
        return 1;
    }
    printf("loaded %u airports\n", (uint32_t) db.airports_count);

    printf("showing first 25 airports\n");
    i = 0;
    while(db.airports[i] && i < 25)
    {
        apt = db.airports[i];
        dumpapt(apt);
        i++;
    }

    printf("showing some cherry picked airports!\n");
    i = 0;
    while(i < DXATC_UTILS_MACROS_ARYSZ(interesting))
    {
        dxAtcAirportDbFind(interesting[i], &apt, &db);
        if(apt)
            dumpapt(apt);
        i++;
    }


    dxAtcAirportDbFree(&db);

    return 0;
}

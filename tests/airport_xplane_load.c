#include <dxatc-engine/airport_xplane.h>
#include <dxatc-engine/latlon.h>
#include <dxatc-utils/macros.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

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
            printf("\t\t\t%s latlon:[%f, %f] length:%.0fft lda:%.0fft\n",
                    rwyend->number, rwyend->latlon[0], rwyend->latlon[1],
                    rwylen * DXATC_UTILS_MACROS_NMI_TO_FT,
                    lda);
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
    const char* xppath = NULL;
    const char* apts = NULL;
    char* apts_cpy = NULL, *apts_ref;
    const char* icao;

    if(argc < 2)
    {
        fprintf(stderr, "use last argv (or -xppath <xplane-path>) to provide xplane location");
        return 1;
    }
    for(int i = 0; i < argc; i++)
    {
        if(strstr("-xppath", argv[i]) && argv[i + 1])
            xppath = argv[i + 1];
        if(strstr("-apts", argv[i]) && argv[i + 1])
            apts = argv[i + 1];

    }
    if(xppath == NULL)
        xppath = argv[argc - 1];

    memset(&db, 0, sizeof(db));
    printf("loading xpdb... ");
    fflush(stdout);
    if(!dxAtcAirportDbXPlaneLoad(xppath, &db))
    {
        fprintf(stderr, "failed to load xplane db from %s\n", argv[1]);
        return 1;
    }
    printf("loaded %u airports\n", (uint32_t) db.airports_count);

    printf("showing first 25 airports\n");
    for(size_t i = 0; i < 25; i++)
    {
        apt = db.airports[i];
        dumpapt(apt);
    }

    printf("showing some cherry picked airports!\n");
    for(size_t i = 0; i < DXATC_UTILS_MACROS_ARYSZ(interesting); i++)
    {
        if(dxAtcAirportDbFind(interesting[i], &apt, &db))
            dumpapt(apt);
    }

    if(apts == NULL)
    {
        printf("you can use -apts [comma_seprated_icao_codes] to list infos about specific airports");
    }else {
        apts_cpy = strdup(apts);
        apts_ref = apts_cpy;
        while((icao = strtok(apts_ref, ",")))
        {
            apt = NULL;
            apts_ref = NULL;
            if(dxAtcAirportDbFind(icao, &apt, &db))
                dumpapt(apt);
        }
        free(apts_cpy);
    }

    dxAtcAirportDbFree(&db);

    return 0;
}

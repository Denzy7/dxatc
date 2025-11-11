#include <dxatc-engine/stdphrase.h>
#include <dxatc-engine/metar.h>
#include <dxatc-utils/macros.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    DxAtcMetar metar;
    char decoded[2048];
    char stdphrase[4096];
    const char* test[] = {
        NULL, /* for metar */
        "Contact tower on 123.45. Bye bye",
        "Cleared to land runway 12",
        "Cleared for takeoff runway 34R",
        "Proceed direct SHIPP",
        "Cleared ILS approach runway 06. JOMOK2C transition",
        "Test Airlines 123AB",
        "Boeing 738 5YCYC",
        "Cessna N172SP",
        "Climb and maintain 16500",
        "Cross TUSKE at altitude 6000",
        "Expect FL240 in 10 minutes",
        NULL /* for argv1 */
    };
    const char** testlast = &test[DXATC_UTILS_MACROS_ARYSZ(test) - 1];

    if(argc > 1)
        *testlast = argv[1];

    if(*testlast == NULL)
        *testlast = "You can use argv1 to supply a string to convert to standard phraseology";

    memset(&metar, 0, sizeof(metar));

    dxAtcMetarInit(&metar);
    dxAtcMetarParse(DXATC_ENGINE_METAR_TEST, &metar);
    dxAtcMetarDecode(decoded, sizeof(decoded), 0, &metar, NULL);
    test[0] = decoded;

    for(size_t i = 0; i <DXATC_UTILS_MACROS_ARYSZ(test); i++)
    {
        dxAtcStdPhrase(test[i], stdphrase, sizeof(stdphrase), 0);
        printf("===\n%s\n===\n+++\n%s\n+++\n\n", test[i], stdphrase);
    }
    dxAtcMetarFree(&metar);
    return 0;
}

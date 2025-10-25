#include <dxatc-engine/metar.h>
#include <dxatc-utils/vvtor.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    DxAtcMetar metar;
    const char* metar_str = "METAR XYAB 011200Z VRB12G15KT 6000 TSRA FEW010 BKN025CB OVC///TCU 21/20 Q0999";
    /* set 1 to overwrite with test data (default metar_str decoded)*/
#if 0
    DxAtcWeatherCloud cloud1 = {DXATC_ENGINE_WEATHER_CLOUD_TYPE_FEW, DXATC_ENGINE_WEATHER_CLOUD_FLAG_NONE, 1000};
    DxAtcWeatherCloud cloud2 = {DXATC_ENGINE_WEATHER_CLOUD_TYPE_BKN, DXATC_ENGINE_WEATHER_CLOUD_FLAG_CB, 2500};
    DxAtcWeatherCloud cloud3 = {DXATC_ENGINE_WEATHER_CLOUD_TYPE_OVC, DXATC_ENGINE_WEATHER_CLOUD_FLAG_TCU, 0};
    DxAtcWeatherCloud* clouds[] = {
        &cloud1, &cloud2, &cloud3, NULL
    };
    DxAtcWeatherCloud** clouds_override;
#endif
    char decoded[2048];

    memset(&metar, 0, sizeof(metar));
    if(argc > 1)
        metar_str = argv[1];

    dxAtcMetarInit(&metar);
    if(!dxAtcMetarParse(metar_str, &metar))
    {
        printf("can't parse metar");
        return 1;
    }

    /* set 1 to overwrite with test data (default metar_str decoded)*/
#if 0
    dxAtcMetarFree(&metar);
    memset(&metar, 0, sizeof(metar));

    snprintf(metar.icao, sizeof(metar.icao), "XYAB");
    snprintf(metar.zulu, sizeof(metar.icao), "1200");

    metar.wind.direction = -1;
    metar.wind.speed = 12;
    metar.wind.gust = 15;
    metar.wind.variable_x = 120;
    metar.wind.variable_y = 300;

    metar.visibility = 6000;

    metar.precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_MODERATE;
    metar.precip.type = DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS | DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA;


    vvtor_init((void***)&clouds_override);
    for(size_t i = 0; i < 3; i++)
    {
        DxAtcWeatherCloud* cloud = calloc(1, sizeof(DxAtcWeatherCloud));
        memcpy(cloud, clouds[i], sizeof(DxAtcWeatherCloud));
        vvtor_push((void***)&clouds_override, cloud);
    }
    metar.sky = clouds_override;

    metar.oat = 21;
    metar.dewpt = 20;
    metar.qnh = 999;
#endif

    dxAtcMetarDecode(decoded, sizeof(decoded), 0, &metar);
    printf("%s", decoded);


    dxAtcMetarFree(&metar);
}

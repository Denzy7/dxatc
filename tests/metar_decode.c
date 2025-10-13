#include <dxatc-engine/metar.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    DxAtcMetar metar;
    memset(&metar, 0, sizeof(metar));
    const char* metar_str = "";
    if(!dxAtcMetarParse(metar_str, &metar))
    {
        printf("can't parse metar");
        return 1;
    }

    snprintf(metar.icao, sizeof(metar.icao), "XYAB");
    snprintf(metar.zulu, sizeof(metar.icao), "1200");

    DxAtcWeatherCloud cloud1 = {DXATC_ENGINE_WEATHER_CLOUD_TYPE_FEW, DXATC_ENGINE_WEATHER_CLOUD_FLAG_NONE, 1000};
    DxAtcWeatherCloud cloud2 = {DXATC_ENGINE_WEATHER_CLOUD_TYPE_BKN, DXATC_ENGINE_WEATHER_CLOUD_FLAG_CB, 2500};
    DxAtcWeatherCloud cloud3 = {DXATC_ENGINE_WEATHER_CLOUD_TYPE_OVC, DXATC_ENGINE_WEATHER_CLOUD_FLAG_TCU, 0};
    DxAtcWeatherCloud* clouds[] = {
        &cloud1, &cloud2, &cloud3, NULL
    };
    metar.sky = clouds;
    metar.precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_MODERATE;
    metar.precip.type = DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS | DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA;
    metar.visibility = 6000;
    metar.wind.direction = -1;
    metar.wind.speed = 12;
    metar.wind.gust = 15;
    metar.wind.variable_x = 120;
    metar.wind.variable_y = 300;
    metar.oat = 21;
    metar.dewpt = 21;
    metar.qnh = 999;

    char wind_gust[64];
    memset(wind_gust, 0, sizeof(wind_gust));
    if(metar.wind.gust)
        snprintf(wind_gust, sizeof(wind_gust), " gusting %d", metar.wind.gust);

    char wind_direction[32];
    memset(wind_direction, 0, sizeof(wind_direction));
    if(metar.wind.direction < 0)
        snprintf(wind_direction, sizeof(wind_direction), "variable");
    else
        snprintf(wind_direction, sizeof(wind_direction), "%d", metar.wind.direction);

    char wind_variable[32];
    memset(wind_variable, 0, sizeof(wind_variable));
    if(metar.wind.variable_x && metar.wind.variable_y)
        snprintf(wind_variable, sizeof(wind_variable), " variable between %d and %d", metar.wind.variable_x, metar.wind.variable_y);

    char wind[128];
    memset(wind, 0, sizeof(wind));
    if(metar.wind.speed <= 3)
    {
        snprintf(wind, sizeof(wind), "light%s", metar.wind.direction < 0 ? " and variable" :"");
    }else {
        snprintf(wind, sizeof(wind), "%s at %d%s%s",
                wind_direction, metar.wind.speed, wind_gust, wind_variable);
    }

    char visibility[32];
    memset(visibility, 0, sizeof(visibility));
    if(metar.visibility >= 9999)
        snprintf(visibility, sizeof(visibility), "more than 10");
    else
        snprintf(visibility, sizeof(visibility), "%d", metar.visibility);

    char precip[128];
    memset(precip, 0, sizeof(precip));
    if(metar.precip.type)
    {
        char intensity[32];
        memset(intensity, 0, sizeof(intensity));
        if(metar.precip.intensity == DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_LIGHT)
            snprintf(intensity, sizeof(intensity), "Light ");
        else if(metar.precip.intensity == DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_MODERATE)
            snprintf(intensity, sizeof(intensity), "Moderate ");
        else if(metar.precip.intensity == DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_HEAVY)
            snprintf(intensity, sizeof(intensity), "Heavy ");

        char type[32];
        memset(type, 0, sizeof(type));
        if(metar.precip.type == DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS){
            snprintf(type, sizeof(type), "thunderstorms");
        }else if(metar.precip.type & DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS && 
                metar.precip.type & DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA){
            snprintf(type, sizeof(type), "thunderstorms and rain");
        }else if(metar.precip.type == DXATC_ENGINE_WEATHER_PRECIP_TYPE_SH)
        {
            snprintf(type, sizeof(type), "showers");
        }else if(metar.precip.type & DXATC_ENGINE_WEATHER_PRECIP_TYPE_SH &&
                metar.precip.type & DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA){
            snprintf(type, sizeof(type), "showers and rain");
        }else if(metar.precip.type == DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA){
            snprintf(type, sizeof(type), "rain");
        }else if(metar.precip.type == DXATC_ENGINE_WEATHER_PRECIP_TYPE_DZ){
            snprintf(type, sizeof(type), "drizzle");
        }
        snprintf(precip, sizeof(precip), "%s%s. ", intensity, type);
    }

    char sky[512];
    memset(sky, 0, sizeof(sky));
    if(metar.sky)
    {
        int nsky = 0;
        size_t wsky = snprintf(sky, sizeof(sky), "conditions. ");
        
        while(metar.sky[nsky] && wsky < sizeof(sky))
        {
            int write = 0;

            char type[16];
            memset(type, 0, sizeof(type));
            if(metar.sky[nsky]->type == DXATC_ENGINE_WEATHER_CLOUD_TYPE_FEW)
                snprintf(type, sizeof(type), "Few");
            else if(metar.sky[nsky]->type == DXATC_ENGINE_WEATHER_CLOUD_TYPE_SCT)
                snprintf(type, sizeof(type), "Scattered");
            else if(metar.sky[nsky]->type == DXATC_ENGINE_WEATHER_CLOUD_TYPE_BKN)
                snprintf(type, sizeof(type), "Broken");
            else if(metar.sky[nsky]->type == DXATC_ENGINE_WEATHER_CLOUD_TYPE_OVC)
                snprintf(type, sizeof(type), "Overcast");

            char flag[32];
            memset(flag, 0, sizeof(flag));
            if(metar.sky[nsky]->flags == DXATC_ENGINE_WEATHER_CLOUD_FLAG_CB)
                snprintf(flag, sizeof(flag), "cumulonimbus");
            else if(metar.sky[nsky]->flags == DXATC_ENGINE_WEATHER_CLOUD_FLAG_TCU)
                snprintf(flag, sizeof(flag), "towering cumulonimbus");
            else
                snprintf(flag, sizeof(flag), "clouds");

            char alt[16];
            memset(alt, 0, sizeof(alt));
            if(metar.sky[nsky]->agl)
                snprintf(alt, sizeof(type), "at %d", metar.sky[nsky]->agl);
            else
                snprintf(alt, sizeof(type), "indefinite");

            write = snprintf(sky + wsky, sizeof(sky) - wsky, "%s %s %s. ", type, flag, alt);

            wsky += write;
            nsky++;
        }
    }else {
        snprintf(sky, sizeof(sky), "clear. ");
    }

    printf("%s weather. Recorded time %s zulu. "
            "Wind %s. "
            "Visibility %s. "
            "%s"
            "Sky %s"
            "Temperature %d. Dewpoint %d. "
            "QNH %d"
            "\n",
            metar.icao, metar.zulu, 
            wind,
            visibility,
            precip,
            sky,
            metar.oat, metar.dewpt,
            metar.qnh
            );
}

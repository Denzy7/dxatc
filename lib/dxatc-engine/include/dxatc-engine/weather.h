#ifndef DXATC_ENGINE_WEATHER_H
#define DXATC_ENGINE_WEATHER_H

typedef enum
{
    DXATC_ENGINE_WEATHER_CLOUD_TYPE_NONE,
    DXATC_ENGINE_WEATHER_CLOUD_TYPE_FEW,
    DXATC_ENGINE_WEATHER_CLOUD_TYPE_SCT,
    DXATC_ENGINE_WEATHER_CLOUD_TYPE_BKN,
    DXATC_ENGINE_WEATHER_CLOUD_TYPE_OVC,

    DXATC_ENGINE_WEATHER_CLOUD_TYPE_COUNT
}DxAtcWeatherCloudType;

typedef enum
{
    DXATC_ENGINE_WEATHER_CLOUD_FLAG_NONE,
    DXATC_ENGINE_WEATHER_CLOUD_FLAG_CB,
    DXATC_ENGINE_WEATHER_CLOUD_FLAG_TCU,

    //DXATC_ENGINE_WEATHER_CLOUD_FLAG_ALL = 0x7fffffff
}DxAtcWeatherCloudFlag;

typedef enum
{
    DXATC_ENGINE_WEATHER_PRECIP_TYPE_NONE = 0,
    DXATC_ENGINE_WEATHER_PRECIP_TYPE_DZ = 1,
    DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA = 2,
    DXATC_ENGINE_WEATHER_PRECIP_TYPE_SH = 1 << 2,
    DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS = 1 << 3,
}DxAtcWeatherPrecipType;

typedef enum
{
    DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_NONE,
    DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_LIGHT,
    DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_MODERATE,
    DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_HEAVY,
}DxAtcWeatherPrecipIntensity;

typedef struct
{
    DxAtcWeatherCloudType type;
    DxAtcWeatherCloudFlag flags;
    int agl;
}DxAtcWeatherCloud;

typedef struct
{
    DxAtcWeatherPrecipType type;
    DxAtcWeatherPrecipIntensity intensity;
    int vicinity;
}DxAtcWeatherPrecip;

typedef struct
{
    int direction, speed, gust;
    int variable_x, variable_y;
}DxAtcWeatherWind;

#endif


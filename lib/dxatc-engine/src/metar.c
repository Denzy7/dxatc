#include "dxatc-engine/metar.h"
#include "dxatc-engine/weather.h"
#include "dxatc-utils/vvtor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum
{
    _DXATC_ENGINE_METAR_PARSE_STAGE_TYPE,
    _DXATC_ENGINE_METAR_PARSE_STAGE_ICAO,
    _DXATC_ENGINE_METAR_PARSE_STAGE_ZULU,
    _DXATC_ENGINE_METAR_PARSE_STAGE_WIND,
    _DXATC_ENGINE_METAR_PARSE_STAGE_WINDVAR,
    _DXATC_ENGINE_METAR_PARSE_STAGE_VISIBILITY,
    _DXATC_ENGINE_METAR_PARSE_STAGE_VISIBILITYCORR,
    _DXATC_ENGINE_METAR_PARSE_STAGE_PRECIP,
    _DXATC_ENGINE_METAR_PARSE_STAGE_SKY,
    _DXATC_ENGINE_METAR_PARSE_STAGE_OATDEWPT,
    _DXATC_ENGINE_METAR_PARSE_STAGE_QNH,

    _DXATC_ENGINE_METAR_PARSE_STAGE_COUNT,
}_DxAtcMetarParseStage;

int _dxAtcMetarParseStage_Type(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_ICAO(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_Zulu(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_Wind(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_WindVar(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_Visibility(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_VisibilityCorr(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_Precip(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_Sky(_DxAtcMetarParseStage* state, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_OATDewPt(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);
int _dxAtcMetarParseStage_QNH(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);

typedef int (*_dxAtcMetarParseStage_Func)(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar);

int dxAtcMetarInit(DxAtcMetar* metar)
{
    return vvtor_init((void***)&metar->sky);
}
void dxAtcMetarFree(DxAtcMetar* metar)
{
    size_t i = 0;
    DxAtcWeatherCloud* cloud = NULL;
    while((cloud = metar->sky[i]))
    {
        free(cloud);
        i++;
    }
    free(metar->sky);
}
int dxAtcMetarParse(const char* string, DxAtcMetar* metar)
{
    char* string_dup = strdup(string);
    char* string_ontok = string_dup;
    const char* tok = NULL;
    _DxAtcMetarParseStage stage = 0;
    int success = 0;

    _dxAtcMetarParseStage_Func parsefuncs[_DXATC_ENGINE_METAR_PARSE_STAGE_COUNT] = 
    {
        _dxAtcMetarParseStage_Type,
        _dxAtcMetarParseStage_ICAO,
        _dxAtcMetarParseStage_Zulu,
        _dxAtcMetarParseStage_Wind,
        _dxAtcMetarParseStage_WindVar,
        _dxAtcMetarParseStage_Visibility,
        _dxAtcMetarParseStage_VisibilityCorr,
        _dxAtcMetarParseStage_Precip,
        _dxAtcMetarParseStage_Sky,
        _dxAtcMetarParseStage_OATDewPt,
        _dxAtcMetarParseStage_QNH,
    };
    /*printf("parse: %s\n", string);*/

    while((tok = strtok(string_ontok, " ")) && stage < _DXATC_ENGINE_METAR_PARSE_STAGE_COUNT)
    {
        string_ontok = NULL;
        if (parsefuncs[stage](&stage, tok, metar))
            success++;
    }
    free(string_dup);
    /*printf("got %d of %d stages\n", success, _DXATC_ENGINE_METAR_PARSE_STAGE_COUNT);*/

    return 1;
}

int _dxAtcMetarParseStage_Type(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    if(!strcmp(tok, "SPECI"))
        metar->type = DXATC_ENGINE_METAR_TYPE_SPECI;

    if(strlen(tok) == 4)
    {
        *stage = *stage + 1;
        return _dxAtcMetarParseStage_ICAO(stage, tok, metar);
    }

    *stage = *stage + 1;
    return 1;
}

int _dxAtcMetarParseStage_ICAO(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    snprintf(metar->icao, sizeof(metar->icao), "%s", tok);
    *stage = *stage + 1;
    return 1;
}
int _dxAtcMetarParseStage_Zulu(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    if(!strchr(tok, 'Z'))
        return 0;

    sscanf(tok, "%*2c%4sZ",metar->zulu);

    *stage = *stage + 1;
    return 1;
}
int _dxAtcMetarParseStage_Wind(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    char dir[4];
    char unit[4];
    int windtok = 0;
    
    if(strstr(tok, "KT") || strstr(tok, "MPS"))
        windtok = 1;

    if(!windtok)
        return 0;

    if(strchr(tok, 'G'))
        sscanf(tok, "%3s%2dG%2d%3s", dir, &metar->wind.speed, &metar->wind.gust, unit);
    else
        sscanf(tok, "%3s%2d%3s", dir, &metar->wind.speed, unit);

    if(!strcmp(dir, "VRB"))
        metar->wind.direction = -1;
    else
        sscanf(dir, "%d", &metar->wind.direction);

    if(!strcmp(unit, "MPS"))
    {
        float spdconv = (float)metar->wind.speed * DXATC_UTILS_MACROS_MPS_TO_KT;
        metar->wind.speed = spdconv;
    }

    *stage = *stage + 1;

    return 1;
}

int _dxAtcMetarParseStage_WindVar(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    if(strlen(tok) == 7 && tok[3] == 'V'){
        sscanf(tok, "%3d%*1c%3d", &metar->wind.variable_x, &metar->wind.variable_y);
    }
    else{
        /* we may have gotten a visibility token */
        *stage = *stage + 1;
        return _dxAtcMetarParseStage_Visibility(stage, tok, metar);
    }

    *stage = *stage + 1;
    return 1;
}
int _dxAtcMetarParseStage_Visibility(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar){
    if(!strcmp(tok, "CAVOK"))
    {
        metar->visibility = 10000;
    }else if(strstr(tok, "SM")){
        float num, denom, visconv;
        if(strchr(tok, '/')){
            sscanf(tok, "%f/%fSM", &num, &denom);
            visconv = num / denom;
        }
        else{
            sscanf(tok, "%fSM", &visconv);
        }
        metar->visibility = visconv * DXATC_UTILS_MACROS_SM_TO_KM * 1000;
    }else {
        sscanf(tok, "%4d", &metar->visibility);
    }

    *stage = *stage + 1;
    return 1;
}

int _dxAtcMetarParseStage_VisibilityCorr(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    /* correct if sm in fractions (WHY THO!)
     * we might have gotten a precip token!*/
    if(!strstr(tok, "SM"))
    {
        *stage = *stage + 1;
        return _dxAtcMetarParseStage_Precip(stage, tok, metar);
    }    

    float num, denom;
    sscanf(tok, "%f/%fSM", &num, &denom);
    metar->visibility = ((float)metar->visibility + (num / denom)) * DXATC_UTILS_MACROS_SM_TO_KM * 1000;

    *stage = *stage + 1;
    return 1;
}
int _dxAtcMetarParseStage_Precip(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    if(strstr(tok, "TS"))
        metar->precip.type |= DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS;
    if(strstr(tok, "SH"))
        metar->precip.type |= DXATC_ENGINE_WEATHER_PRECIP_TYPE_SH;
    if(strstr(tok, "RA"))
        metar->precip.type |= DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA;
    if(strstr(tok, "DZ"))
        metar->precip.type |= DXATC_ENGINE_WEATHER_PRECIP_TYPE_DZ;
    if(strstr(tok, "VC"))
        metar->precip.vicinity = 1;

    if(metar->precip.type){
        metar->precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_MODERATE;
        if(strchr(tok, '+'))
            metar->precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_HEAVY;
        else if(strchr(tok, '-'))
            metar->precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_LIGHT;
    }else{
        /* we may have received a sky token */
        *stage = *stage + 1;
        return _dxAtcMetarParseStage_Sky(stage, tok, metar);
    };

    *stage = *stage + 1;
    return 1;
}
int _dxAtcMetarParseStage_Sky(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    if(!metar->sky)
    {
        printf("metar must be init to parse sky conditions\n");
        *stage = *stage + 1;
        return 0;
    }

    /* parse up to temperature */
    int slashes = 0;
    for(size_t i = 0; i < strlen(tok); i++)
    {
        if(tok[i] == '/')
            slashes++;
    }
    if(slashes == 1 || metar->visibility == 10000 || strstr(tok, "CLR"))
    {
        *stage = *stage + 1;
        return _dxAtcMetarParseStage_OATDewPt(stage, tok, metar);
    }

    char type[4];
    DxAtcWeatherCloud cloud;
    memset(&cloud, 0, sizeof(cloud));
    sscanf(tok, "%3s%3d%*3c", type, &cloud.agl);

    if(!strcmp(type, "FEW"))
        cloud.type = DXATC_ENGINE_WEATHER_CLOUD_TYPE_FEW;
    else if(!strcmp(type, "SCT"))
        cloud.type = DXATC_ENGINE_WEATHER_CLOUD_TYPE_SCT;
    else if(!strcmp(type, "BKN"))
        cloud.type = DXATC_ENGINE_WEATHER_CLOUD_TYPE_BKN;
    else if(!strcmp(type, "OVC"))
        cloud.type = DXATC_ENGINE_WEATHER_CLOUD_TYPE_OVC;

    if(strstr(tok, "CB"))
        cloud.flags = DXATC_ENGINE_WEATHER_CLOUD_FLAG_CB;
    else if(strstr(tok, "TCU"))
        cloud.flags = DXATC_ENGINE_WEATHER_CLOUD_FLAG_TCU;

    if(slashes == 3)
        cloud.agl = 0;

    cloud.agl *= 100;

    if(!cloud.type){
        /* could be an UP token */
        return 0;
    }else {
        DxAtcWeatherCloud* _c = calloc(1, sizeof(DxAtcWeatherCloud));
        memcpy(_c, &cloud, sizeof(cloud));
        vvtor_push((void***)&metar->sky, _c);
    }

    return 1;

}

void _dxAtcMetarParseState_OATDewPt_ParseTemp(const char* temp, int* dest)
{
    int mul = 1;
    const char* realtemp = temp;
    if(strchr(temp, 'M'))
    {
        mul = -1;
        realtemp = temp + 1;
    }

    sscanf(realtemp, "%d", dest);
    *dest*= mul;
}
int _dxAtcMetarParseStage_OATDewPt(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    const char* dewpt = strchr(tok, '/');
    if(!dewpt)
        return 0;

    dewpt+=1;
    char* temp = strdup(tok);
    temp[strlen(temp) - strlen(dewpt) - 1] = 0;

    _dxAtcMetarParseState_OATDewPt_ParseTemp(temp, &metar->oat);
    _dxAtcMetarParseState_OATDewPt_ParseTemp(dewpt, &metar->dewpt);

    free(temp);
    *stage = *stage + 1;
    return 1;
}
int _dxAtcMetarParseStage_QNH(_DxAtcMetarParseStage* stage, const char* tok, DxAtcMetar* metar)
{
    if(tok[0] == 'Q')
    {
        sscanf(tok, "Q%4d", &metar->qnh);
    }else if(tok[0] == 'A')
    {
        sscanf(tok, "A%4d", &metar->qnh);
        float qnhconv = DXATC_UTILS_MACROS_INHG_TO_HPA * ((float)metar->qnh / 100);
        metar->qnh = qnhconv;
    }
    *stage = *stage + 1;
    return 1;
}


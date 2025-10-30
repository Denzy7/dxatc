#include "dxatc-engine/metar_xplane.h"
#include "dxatc-engine/latlon.h"

#include "dxatc-utils/socket.h"
#include "dxatc-utils/vvtor.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h> /* socket */
#include <arpa/inet.h> /* inet_aton */
#include <netdb.h> /* getaddrinfo */
#endif

#define _DXATC_ENGINE_METAR_XPLANE_PORT 49000

typedef int xint;
typedef char xchr;
typedef float xflt;
/* we use drefs to fetch, more precise weather even
 * when interpolated*/
struct dref_struct_in
{
	xint dref_freq;
	xint dref_sender_index;
	xchr dref_string[400];
};

struct dref_struct_out
{
	xint dref_sender_index	;
	xflt dref_flt_value		;
};

struct dref_array
{
    const char* const dref;
    const int n;
    float* out;
};

int dxAtcMetarXPlaneFetch(const char* ip, DxAtcMetar* metar, const DxAtcAirportDb* db)
{
    float cloud_type[3], cloud_base[3];
    /* we only need the first component,
     * we'll get all 3 to make xplane happy
     */
    float wind_alt[3], wind_dir[3], wind_speed[3], wind_shear_dir[3], wind_shear_speed[3];
    float rain_pc, thunderstorm_pc;
    float baro_inhg;
    float temp, dewpt;
    float vis;
    DxAtcLatLon acftlatlon;

    struct dref_array drefs[] =
    {
        {"sim/weather/cloud_type", 3, cloud_type},
        {"sim/weather/cloud_base_msl_m", 3, cloud_base},
        {"sim/weather/wind_altitude_msl_m", 3, wind_alt},
        {"sim/weather/wind_direction_degt", 3, wind_dir},
        {"sim/weather/wind_speed_kt", 3, wind_speed},
        {"sim/weather/shear_direction_degt", 3, wind_shear_dir},
        {"sim/weather/shear_speed_kt", 3, wind_shear_speed},
        {"sim/weather/rain_percent", 1, &rain_pc},
        {"sim/weather/thunderstorm_percent", 1, &thunderstorm_pc},
        {"sim/weather/barometer_sealevel_inhg", 1, &baro_inhg},
        {"sim/weather/temperature_ambient_c", 1, &temp},
        {"sim/weather/dewpoi_sealevel_c", 1, &dewpt}, /* hmmm? */
        {"sim/weather/visibility_reported_m", 1, &vis},
        {"sim/flightmodel/position/latitude", 1, &acftlatlon[0]},
        {"sim/flightmodel/position/longitude", 1, &acftlatlon[1]},
    };

    const char* ip_default = "localhost";
    char ip_resolved[64];
    struct addrinfo hints, *res = NULL;
    int status;
    struct sockaddr_in xpaddr;
#ifdef _WIN32
    WORD wsa_ver = 0;
    WSADATA wsa_data;
#endif
    DxAtcSocket sock = DXATC_UTILS_SOCKET_INVALID, testsock = DXATC_UTILS_SOCKET_INVALID;
    struct sockaddr_in bindaddr;
    struct dref_struct_in** drefs_vtor;
    struct dref_array** drefs_linear;
    struct dref_array* dref_i;
    size_t i = 0, n = 0;
    int j = 0;
    int wind_shear_dir0;
    struct dref_struct_in* dref;
    struct dref_struct_out* dref_out;
    size_t recvbuf_sz;
    void* recvbuf;
    char sendbuf[5 + sizeof(struct dref_struct_in)];

    DxAtcWeatherCloud clouds[3];
    DxAtcWeatherCloud* cloud = NULL;

    DxAtcAirport* findapt = NULL;
    int msl = 0;
    float shortest = 999.0f, disttoplane;
    time_t zulu;

    if(!metar->sky)
    {
        fprintf(stderr, "metar must be init to fetch from xplane");
        return 0;
    }

    if(ip && strlen(ip))
         ip_default = ip;

    testsock = socket(AF_INET, SOCK_DGRAM, 0);
    if(testsock == DXATC_UTILS_SOCKET_INVALID)
    {
#ifdef _WIN32
        wsa_ver = MAKEWORD(2, 2);
        if(WSAStartup(wsa_ver, &wsa_data) != 0)
        {
            fprintf(stderr, "WSAStartup failed: err=%d\n", WSAGetLastError());
            return 0;
        }

        if(HIBYTE(wsa_data.wVersion) != 2 || LOBYTE(wsa_data.wVersion) != 2)
        {
            fprintf(stderr, "WinSock2 unavailable: err=%d\n", WSAGetLastError());
            WSACleanup();
            return 0;
        }
        testsock = socket(AF_INET, SOCK_DGRAM, 0);
#endif
    }
    if(testsock == DXATC_UTILS_SOCKET_INVALID)
    {
        fprintf(stderr, "failed to create test socket!\n");
        return 0;
    }
    dxAtcSocketClose(testsock);

    memset(&hints, 0, sizeof(hints));
    memset(&xpaddr, 0, sizeof(xpaddr));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if((status = getaddrinfo(ip_default, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 0;
    }
    xpaddr.sin_family = AF_INET;
    xpaddr.sin_port = htons(_DXATC_ENGINE_METAR_XPLANE_PORT);
    memcpy(&xpaddr.sin_addr, &((struct sockaddr_in*)res->ai_addr)->sin_addr, sizeof(xpaddr.sin_addr));
    freeaddrinfo(res);
    snprintf(ip_resolved, sizeof(ip_resolved), "%s", inet_ntoa(xpaddr.sin_addr));

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == DXATC_UTILS_SOCKET_INVALID)
    {
        fprintf(stderr, "cant create socket\n");
        return 0;
    }

    memset(&bindaddr, 0, sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    if(bind(sock, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) < 0)
    {
        perror("bind");
        return 0;
    }

    /* prepare data */
    if(!vvtor_init((void***)&drefs_vtor))
    {
        fprintf(stderr, "cant init vvtor\n");
        return 0;
    }

    if(!vvtor_init((void***)&drefs_linear))
    {
        fprintf(stderr, "cant init vvtor\n");
        return 0;
    }

    for(i = 0; i < DXATC_UTILS_MACROS_ARYSZ(drefs); i++)
    {
        for(j = 0; j < drefs[i].n; j++)
        {
            dref = calloc(1, sizeof(struct dref_struct_in));
            if(drefs[i].n == 1)
                snprintf(dref->dref_string, sizeof(dref->dref_string), "%s", drefs[i].dref);
            else
                snprintf(dref->dref_string, sizeof(dref->dref_string), "%s[%d]", drefs[i].dref, j);

            dref->dref_freq = 1;
            dref->dref_sender_index = n;
            vvtor_push((void***)&drefs_vtor, dref);

            dref_i = calloc(1, sizeof(struct dref_array));
            memcpy(dref_i, &drefs[i], sizeof(struct dref_array));
            dref_i->out = &drefs[i].out[j];
            vvtor_push((void***)&drefs_linear, dref_i);
            n++;
        }
    }

    recvbuf_sz = 5 + (n * sizeof(struct dref_struct_out));
    recvbuf = malloc(recvbuf_sz);

    i = 0;
    while(drefs_vtor[i])
    {
        snprintf(sendbuf, sizeof(sendbuf), "%s", "RREF");
        dref = (struct dref_struct_in*)(sendbuf + 5);
        memcpy(dref, drefs_vtor[i], sizeof(struct dref_struct_in));
        sendto(sock, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&xpaddr, sizeof(xpaddr));
        i++;
    }

    printf("waiting for xplane to respond from %s (%s) port %d\n", ip_default, ip_resolved, _DXATC_ENGINE_METAR_XPLANE_PORT);
    recvfrom(sock, recvbuf, recvbuf_sz, 0, NULL, NULL);
    dref_out = recvbuf + 5;
    i = 0;
    while(i < n)
    {
        /* tell xplane to stop sending */
        snprintf(sendbuf, sizeof(sendbuf), "%s", "RREF");
        dref = (struct dref_struct_in*)(sendbuf + 5);
        memcpy(dref, drefs_vtor[i], sizeof(struct dref_struct_in));
        dref->dref_freq = 0;
        sendto(sock, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&xpaddr, sizeof(xpaddr));

        *drefs_linear[i]->out = dref_out[i].dref_flt_value;

        i++;
    }

    /* find nearest apt */
    if(db){
        for(size_t i = 0; i < db->airports_count; i++)
        {
            disttoplane = dxAtcLatLonDistanceNMI(acftlatlon, db->airports[i]->latlon);
            if(disttoplane < shortest)
            {
                dxAtcAirportDbFind(db->airports[i]->icao, &findapt, db);
                shortest = disttoplane;
            }
        }
    }

    memset(clouds, 0, sizeof(clouds));
    if(findapt){
        memcpy(metar->icao, findapt->icao, sizeof(metar->icao));
        msl = findapt->elev_msl;
    }

    time(&zulu);
    strftime(metar->zulu, sizeof(metar->zulu),"%H%M", gmtime(&zulu));

    /* decode */
    for(i = 0; i < 3; i++)
    {
        clouds[i].type = cloud_type[i];
        clouds[i].agl = (cloud_base[i] * DXATC_UTILS_MACROS_M_TO_FT) - msl;
        clouds[i].agl /= 100;
        clouds[i].agl *= 100;

        if(clouds[i].type)
        {
            cloud = calloc(1, sizeof(DxAtcWeatherCloud));
            memcpy(cloud, &clouds[i], sizeof(DxAtcWeatherCloud));
            vvtor_push((void***)&metar->sky, cloud);
        }
    }

    metar->wind.direction = wind_dir[0];
    if(metar->wind.direction == 0)
        metar->wind.direction = 360;
    metar->wind.direction /= 10;
    metar->wind.direction *= 10;
    metar->wind.speed = wind_speed[0];
    if(wind_shear_speed[0] > 1.0f)
        metar->wind.gust = wind_speed[0] + wind_shear_speed[0];
    /*TODO: check if 30 is enough to consider variable
     * xplane consideres it enough*/
    if(wind_shear_dir[0] > 30.0f)
    {
        wind_shear_dir0 = wind_shear_dir[0];
        wind_shear_dir0 /= 10;
        wind_shear_dir0 *= 10;
        metar->wind.variable_x = metar->wind.direction - wind_shear_dir0;
        metar->wind.variable_y = metar->wind.direction + wind_shear_dir0;
        if(metar->wind.variable_y >= 360)
            metar->wind.variable_y -= 360;
    }
    metar->dewpt = dewpt;
    metar->oat = temp;
    metar->visibility = vis;
    metar->qnh = baro_inhg * DXATC_UTILS_MACROS_INHG_TO_HPA;

    if(thunderstorm_pc)
        metar->precip.type |= DXATC_ENGINE_WEATHER_PRECIP_TYPE_TS;
    if(rain_pc)
        metar->precip.type |= DXATC_ENGINE_WEATHER_PRECIP_TYPE_RA;

    if(rain_pc || thunderstorm_pc)
        metar->precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_MODERATE;
    if(rain_pc > 0.65f || thunderstorm_pc > 0.65f)
        metar->precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_HEAVY;
    if(rain_pc < 0.25f || thunderstorm_pc < 0.25f)
        metar->precip.intensity = DXATC_ENGINE_WEATHER_PRECIP_INTENSITY_LIGHT;


    free(recvbuf);
    i = 0;
    while(drefs_vtor[i])
    {
        free(drefs_vtor[i]);
        free(drefs_linear[i]);
        i++;
    }
    free(drefs_vtor);
    free(drefs_linear);
    dxAtcSocketClose(sock);

#ifdef _WIN32
    if(wsa_ver)
        WSACleanup();
#endif
    return 1;

}

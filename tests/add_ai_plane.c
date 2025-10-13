#include <XPLMPlugin.h>
#include <XPLMPlanes.h>
#include <XPLMUtilities.h>
#include <stdio.h>

PLUGIN_API int XPluginStart(char * outName, char * outSignature, char * outDescription)
{
    snprintf(outName, 256, "add_ai_plane");
    snprintf(outSignature, 256, "com.denzygames.dxatc.tests.add_ai_plane");
    snprintf(outDescription, 256, "Add an AI plane and control it");
    return 1;
}

PLUGIN_API void XPluginStop(void)
{
}

void AcquirePlanes(void* arg)
{
    if(!XPLMAcquirePlanes(NULL, &AcquirePlanes, NULL))
    {
        int acfs, acfs_load;
        XPLMPluginID controller;
        char msg[256], plugname[256];
        XPLMCountAircraft(&acfs, &acfs_load, &controller);
        XPLMGetPluginInfo(controller, plugname, NULL, NULL, NULL);
        snprintf(msg, sizeof(msg), "Error: Planes are currently owned by plugin [ %s ]. Please disable this plugin\n", plugname);
        XPLMDebugString(msg);
        return;
    }
    XPLMDebugString("My airplanes!\n");
}

PLUGIN_API int XPluginEnable(void)
{
    /*XPLMLook*/
    AcquirePlanes(NULL);
    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMessage, void * inParam)
{
    if(inMessage == XPLM_MSG_PLANE_LOADED)
    {
        int* id = inParam;
        char msg[512];
        snprintf(msg, sizeof(msg), "load aircraft %d\n", *id);
        XPLMDebugString(msg);
    }
}

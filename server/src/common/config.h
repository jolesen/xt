#ifndef _COMMON_CONFIG_H_
#define _COMMON_CONFIG_H_

#include "core/core.h"

const string kServiceCenter = "center";
const string kServiceGate   = "gate";
const string kServiceAuth   = "auth";
const string kServiceData   = "data";
const string kServiceLogic  = "logic";

// CConfig
class CConfig
{
public:
    string Parse(uint argc, const char **argv);
    void Show();

public:
    string type;
    string sid;
    string resource;
    string config;
    std::map<string, SHost> centers;
    SHost host;
    SHost httpHost;
    SHost mongoHost;
    SHost redisHost;
    uint  clientLimit;
    uint  backlog;
    uint  broadcastMax;
    uint  monitorTick;
};
#define theConfig CSingleton<CConfig>::Instance()

#endif

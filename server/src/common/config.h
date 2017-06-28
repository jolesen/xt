#ifndef _COMMON_CONFIG_H_
#define _COMMON_CONFIG_H_

#include "core/core.h"

const std::string kServiceCenter = "center";
const std::string kServiceGate   = "gate";
const std::string kServiceAuth   = "auth";
const std::string kServiceData   = "data";
const std::string kServiceLogic  = "logic";

// CConfig
class CConfig
{
public:
    std::string Parse(uint argc, const char **argv);
    void Show();

public:
    std::string type;
    std::string sid;
    std::string resource;
    std::string config;
    std::map<std::string, SHost> centers;
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

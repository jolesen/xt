#ifndef _COMMON_CORE_SCONFIG_H_
#define _COMMON_CORE_SCONFIG_H_

#include "client.h"

// CServerConfig
class CServerConfig
{
public:
    typedef void (*FunMsgHandler)(CClient &client, char* buff, uint size);
    typedef uint (*FunMsgIdGetter)(const std::string &msg_name);
    typedef std::string (*FunMsgNameGetter)(uint msg_id);

    std::string      type;
    std::string      sid;
    FunMsgHandler    msg_handler;
    FunMsgIdGetter   msg_id_getter;
    FunMsgNameGetter msg_name_getter;
    SHost            host;
    SHost            http_host;
    SHost            mongo_host;
    SHost            redis_host;
    uint             backlog;
    uint             client_limit;
    uint             monitor_tick;

private:
    friend CServerConfig& CSingleton<CServerConfig>::Instance();
    ~CServerConfig(){}
    CServerConfig()
    {
        type         = "";
        sid          = "";
        backlog      = 0;
        client_limit = 0;
        monitor_tick = 0;
    }
};
#define theServerConfig CSingleton<CServerConfig>::Instance()

#endif

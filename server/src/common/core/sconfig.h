#ifndef _COMMON_CORE_SCONFIG_H_
#define _COMMON_CORE_SCONFIG_H_

#include "client.h"

// CServerConfig
class CServerConfig
{
public:
    typedef void (*FunMsgHandler)(CClient &client, char* buff, uint size);
    typedef uint (*FunMsgIdGetter)(const std::string &msgName);
    typedef std::string (*FunMsgNameGetter)(uint msgId);

    std::string      type;
    std::string      sid;
    FunMsgHandler    funHandleMsg;
    FunMsgIdGetter   funGetMsgId;
    FunMsgNameGetter funGetMsgName;
    SHost            host;
    SHost            httpHost;
    SHost            mongoHost;
    SHost            redisHost;
    uint             backlog;
    uint             clientLimit;
    uint             monitorTick;

private:
    friend CServerConfig& CSingleton<CServerConfig>::Instance();
    ~CServerConfig(){}
    CServerConfig()
    {
        type        = "";
        sid         = "";
        backlog     = 0;
        clientLimit = 0;
        monitorTick = 0;
    }
};
#define theServerConfig CSingleton<CServerConfig>::Instance()

#endif

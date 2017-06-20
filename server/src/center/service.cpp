#include "common/core/server.h"
#include "common/core/linker.h"
#include "common/core/log.h"
#include "common/misc.h"
#include "service.h"

// ----------------------------------------
CServiceManager::CServiceManager()
{
}

CServiceManager::~CServiceManager()
{
}

MSG_FUNC(QSysRegistService)
{
    theServiceManager.AddService(msg.service);
    LOG_INFO("service[%s, %s:%d] registered.", msg.service.sid.c_str(), msg.service.ip.c_str(), msg.service.port);

    CClient* client = theServer.GetClientByFd(msg.header.fd);
    if(!client)
    {
        LOG_ERROR("can not find client by fd:%u", msg.header.fd);
        return;
    }

    RSysRegistService rsp;
    rsp.flag = true;
    RSP(msg, rsp);

    // 建立内部连接
    SHost host(msg.service.ip, msg.service.port);
    theLinker.Link(msg.service.sid, host);
}

#include "common/core/linker.h"
#include "common/core/timer.h"
#include "common/core/log.h"
#include "common/misc.h"
#include "service.h"

EVENT_FUNC(SEventLinkConnected)
{
    const SHost &host = ev.client.GetHost();
    LOG_INFO("connected to host[%s, %s:%u], fd=%u", ev.client.GetServerId().c_str(), host.ip.c_str(), host.port, ev.client.GetFd());
}

bool CheckInnerClient(const STimerInfo &info)
{
    theLinker.CheckLink();

    // test say hello
    QSysSayHello msg;
    const CServiceManager::StringServiceMap &services = theServiceManager.GetServices();
    uint rand = CUtil::Rand(2) + 1;
    FOR(rand, i)
    {
        FORMAP(services, iter)
        {
            if(CUtil::Rand(10) <= 2)
            {
                continue;
            }
            CClient* link = theLinker.GetLink(iter->first);
            if(!link)
            {
                continue;
            }

            msg.list.clear();
            uint count = CUtil::Rand(50000) + 10000;
            FOR(count, i)
            {
                msg.list.push_back(i);
            }
            link->Write(&msg);
        }
    }

    return true;
}

bool Init()
{
    theTimer.AddSecond(1, 0, CheckInnerClient);
    return true;
}

MSG_FUNC(RSysSayHello)
{
}

int main(int argc, const char **argv)
{
    return CMisc::Main(argc, argv, NULL, Init);
}

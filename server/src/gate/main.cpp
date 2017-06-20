#include "common/core/server.h"
#include "common/core/linker.h"
#include "common/core/timer.h"
#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/misc.h"

// ----------------------------------------------------------------------------
/*
void CMisc::OnClientClosed(uint reason, const CConnection *con)
{
    LOG_DEBUG("closefd..... reason=%u", reason);
    if(!con)
    {
        return;
    }

    if(con->GetType() == kConnectionInner)
    {
        // TODO
        return;
    }

    if(con->GetGuid() != 0) // if logined
    {
        RSysUserLogout rsp;
        rsp.uid    = con->GetGuid();
        rsp.reason = reason;
        SEND_LOGIC(rsp);
    }
}
*/

EVENT_FUNC(SEventLinkConnected)
{
    CMisc::OnConnected(ev.client);
}

bool CheckInnerClient(const STimerInfo &info)
{
    theLinker.CheckLink();

    return true;
}

bool Init()
{
    theTimer.AddSecond(1, 0, CheckInnerClient);

    return true;
}

// ----------------------------------------------------------------------------
MSG_FUNC(QSysSayHello)
{
    RSysSayHello rsp;
    RSP(msg, rsp);
}

int main(int argc, const char **argv)
{
    return CMisc::Main(argc, argv, NULL, Init);
}

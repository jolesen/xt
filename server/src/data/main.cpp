#include "common/core/server.h"
#include "common/core/linker.h"
#include "common/core/timer.h"
#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/misc.h"
#include "cacher.h"

// ----------------------------------------------------------------------------
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

    theCacher.Start();

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

#include "common/core/server.h"
#include "common/core/linker.h"
#include "common/core/timer.h"
#include "common/core/time.h"
#include "common/core/event.h"
#include "common/core/log.h"
#include "common/core/bits.h"
#include "common/msg/system.h"
#include "common/misc.h"
#include "common/data/activity.h"

// ----------------------------------------------------------------------------
EVENT_FUNC(SEventLinkConnected)
{
    CMisc::OnConnected(ev.client);
}

MSG_FUNC(RSysRegistService)
{
    LOG_INFO("msg registered, flag:%u", msg.flag);
}

bool TestTimer(const STimerInfo &info)
{
    return true;
}

bool CheckInnerClient(const STimerInfo &info)
{
    theLinker.CheckLink();
    return true;
}

bool Init()
{
    theTimer.AddSecond(1, 0, CheckInnerClient);
    theTimer.AddSecond(1, 10, TestTimer);
    return true;
}

bool LoadData(const string &path)
{
    return theActivityData.Load(path);
}

// ---------------------------------------------------------
MSG_FUNC(QSysSayHello)
{
    RSysSayHello rsp;
    RSP(msg, rsp);
}

int main(int argc, const char **argv)
{
    return CMisc::Main(argc, argv, LoadData, Init);
}

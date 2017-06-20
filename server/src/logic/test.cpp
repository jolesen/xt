#include "common/core/timer.h"
#include "common/core/time.h"
#include "common/core/util.h"
#include "common/core/log.h"
#include "common/msg/test.h"
#include "common/data/value.h"
#include "common/data/user.h"
#include "common/data/activity_const.h"
#include "common/misc.h"
#include "activity.h"
#include "login.h"
#include "test.h"
#include "user.h"

bool CTester::Init()
{
    uint timer = theTimer.AddSecond(1, 0, CTester::TestModify);
    return (timer != 0);
}

void RandIntList(UIntList &list, uint max)
{
    if(list.size() == 0)
    {
        FOR(max, i)
        {
            list.push_back(CUtil::Rand(0, 10000000));
        }
    }
    else
    {
        FORLIST(list, i)
        {
            list[i] = CUtil::Rand(0, 20000000);
        }
    }
}

bool CTester::TestModify(const STimerInfo &info)
{
    user::UserMap &users = user::GetUsers();
    FORMAP(users, iter)
    {
        RandIntList(iter->second.vars.list, kValueVarMax);
        RandIntList(iter->second.times.list, kValueTimeMax);
    }

    return true;
}

// ----------------------------------------------
uint ts_begin    = 0;
uint ts_show     = 0;
uint ls_packets  = 0;
ulong total_count = 0;
MSG_USER(QTestTime)
{
    uint key;
    std::string value;
    FORMAP(msg.imap, iter)
    {
        key   = iter->first;
        value = iter->second;

        break;
    }
    //LOG_DEBUG("uid=%u,time=%u,seq=%u,s=%s,tst=%s,mk=%u, mv=%s", msg.header.guid, msg.time, msg.seq, msg.s.c_str(), msg.tst.c_str(), key, value.c_str());

    // test send
    RTestTime rsp;
    rsp.time = CTime::Now();
    rsp.seq  = msg.seq;
    SEND_USER(user, rsp);

    // test send error
    SEND_USER_ERROR(user, kErrorLoginedOnLogic);
    SEND_USER_ERROR(user, kErrorLoginedOnLogic);

    // test broadcast all
    BROADCAST_ALL(rsp);

    // test broadcast given
    UIntList list;
    //list.push_back(msg.header.guid);
    BROADCAST_LIST(rsp, list);

    // test event
    EVENT(SEventLogin, user);

    // SHOW
    ++total_count;
    uint now = CTime::Now();
    if(ts_begin == 0)
    {
        ts_begin = now;
    }

    if(now - ts_show > 0)
    {
        if(now - ts_begin > 0)
        {
            LOG_INFO("DUR=%u, PK=%llu, LSP=%u, APPS=%u", (now - ts_begin), total_count, total_count - ls_packets, total_count / (now - ts_begin));

            ts_show    = now;
            ls_packets = total_count;
        }
    }
}

// ----------------------------------------------
ACTIVITY_FUNC(kActivityTest, kActivityOpen)
{
}

ACTIVITY_FUNC(kActivityTest, kActivityClose)
{
}

ACTIVITY_FUNC(kActivityTest, kActivityLogin)
{
}

ACTIVITY_FUNC(kActivityTest, kActivityLogout)
{
}

ACTIVITY_FUNC(kActivityTest, kActivityCrossDay)
{
}

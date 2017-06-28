#include "monitor.h"
#include "timer.h"
#include "time.h"
#include "log.h"

#define CHECK_INTERVAL   if(interval == 0) { LOG_ERROR("error, interval = 0"); return 0; }
#define CHECK_WEEKDAY(D) if(D > 6)  { LOG_ERROR("error weekday=%u", D); return 0; }
#define CHECK_HOUR(H)    if(H > 23) { LOG_ERROR("error hour=%u",    H); return 0; }
#define CHECK_MINUTE(M)  if(M > 59) { LOG_ERROR("error minute=%u",  M); return 0; }
#define CHECK_SECOND(S)  if(S > 59) { LOG_ERROR("error second=%u",  S); return 0; }

CTimer::CTimer()
{
    mSeq = 0;
}

CTimer::~CTimer()
{
}

uint CTimer::AddSecond(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickSecond, S2UInt(SECOND * interval, SECOND * interval), loops, S3UInt(), cb);
}

uint CTimer::AddMinute(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickMinute, S2UInt(MINUTE * interval, MINUTE * interval), loops, S3UInt(), cb);
}

uint CTimer::AddHour(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickHour, S2UInt(HOUR * interval, HOUR * interval), loops, S3UInt(), cb);
}

uint CTimer::AddDay(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickDay, S2UInt(DAY * interval, DAY * interval), loops, S3UInt(), cb);
}

uint CTimer::AddMinuteAt(uint interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_SECOND(at.type);

    STime now = CTime::Parse(CTime::Now());

    uint nowSecond = now.second;
    uint atSecond  = at.type;

    S2UInt s(0, (MINUTE * interval));
    if(atSecond >= nowSecond)
    {
        s.id = atSecond - nowSecond;
    }
    else
    {
        s.id = (MINUTE - nowSecond) + ((interval - 1) * MINUTE) + atSecond;
    }

    return CreateTick(ETickMinuteAt, s, loops, at, cb);
}

uint CTimer::AddHourAt(uint interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_MINUTE(at.type);
    CHECK_SECOND(at.id);

    STime now = CTime::Parse(CTime::Now());

    uint nowSecond = (now.minute * MINUTE) + now.second;
    uint atSecond  = (at.type    * MINUTE) + at.id;

    S2UInt s(0, (HOUR * interval));
    if(atSecond >= nowSecond)
    {
        s.id = (atSecond - nowSecond);
    }
    else
    {
        s.id = (HOUR - nowSecond) + ((interval - 1) * HOUR) + atSecond;
    }

    return CreateTick(ETickHourAt, s, loops, at, cb);
}

uint CTimer::AddDayAt(uint interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_HOUR(at.type);
    CHECK_MINUTE(at.id);
    CHECK_SECOND(at.value);

    STime now = CTime::Parse(CTime::Now());

    uint nowSecond = (now.hour * HOUR) + (now.minute * MINUTE) + now.second;
    uint atSecond  = (at.type  * HOUR) + (at.id      * MINUTE) + at.value;

    S2UInt s(0, (DAY * interval));
    if(atSecond >= nowSecond)
    {
        s.id = (atSecond - nowSecond);
    }
    else
    {
        s.id = (DAY - nowSecond) + ((interval - 1) * DAY) + atSecond;
    }

    return CreateTick(ETickDayAt, s, loops, at, cb);
}

uint CTimer::AddWeekDayAt(uint interval, uint loops, EWeekDay day, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_WEEKDAY(day);
    CHECK_HOUR(at.type);
    CHECK_MINUTE(at.id);
    CHECK_SECOND(at.value);

    STime now = CTime::Parse(CTime::Now());

    uint nowWday = now.wday;
    uint atWday  = (uint)day;

    uint nowSecond = (nowWday * DAY) + (now.hour * HOUR) + (now.minute * MINUTE) + now.second;
    uint atSecond  = (atWday  * DAY) + (at.type  * HOUR) + (at.id      * MINUTE) + at.value;

    S2UInt s(0, (WEEK * interval));
    if(atSecond >= nowSecond)
    {
        s.id = (atSecond - nowSecond);
    }
    else
    {
        s.id = (WEEK - nowSecond) + ((interval - 1) * WEEK) + atSecond;
    }

    return CreateTick(ETickWeekDayAt, s, loops, at, cb);
}
    
void CTimer::OnTimeout(evutil_socket_t fd, short event, void *args)
{
    STimerEvent *evt = (STimerEvent*)args;
    if(!evt)
    {
        LOG_ERROR("Cannot get event, fd=%u, event=%u", fd, event);
        return;
    }

    ++(evt->info.ticked);
    MONITOR(OnTimerTicked());

    // 回调处理
    if(!evt->callback(evt->info))
    {
        theTimer.Delete(evt->info.id);
        return;
    }

    // 超过次数
    if((evt->info.loops > 0) && (evt->info.ticked >= evt->info.loops))
    {
        theTimer.Delete(evt->info.id);
        return;
    }

    // 对于固定触发点的定时器，第一次回调后，把这个定时器转换为永久触发方式
    if((evt->info.at.type > 0) && (evt->info.ticked == 1))
    {
        timeval normal;
        normal.tv_sec  = evt->info.interval.value;
        normal.tv_usec = 0;

        evtimer_del(evt->lbevent);
        event_assign(evt->lbevent, theTimer.mBase, -1, EV_PERSIST, CTimer::OnTimeout, evt);
        evtimer_add(evt->lbevent, &normal);
        LOG_DEBUG("Convert timer=%u to be persist.", evt->info.id);
    }
}

void CTimer::Delete(uint id)
{
    IF_NFIND(mEvents, id, iter)
    {
        LOG_ERROR("Can't find event, id=%u", id);
        return;
    }

    STimerEvent *evt = iter->second;
    evtimer_del(evt->lbevent);
    mEvents.erase(evt->info.id);
    delete evt;
    LOG_DEBUG("Delete timer:%u", id);

    MONITOR(OnTimerAddOrDelete(false));
}

uint CTimer::CreateTick(ETick type, const S2UInt &interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    STimerEvent *evt = new STimerEvent;
    if(at.type > 0) // is tick at
    {
        evt->lbevent = evtimer_new(mBase, CTimer::OnTimeout, evt);
    }
    else
    {
        evt->lbevent = event_new(mBase, -1, EV_PERSIST, CTimer::OnTimeout, evt);
    }
    evt->callback = cb;

    evt->info.id       = (++mSeq);
    evt->info.type     = type;
    evt->info.loops    = loops;
    evt->info.interval = interval;
    evt->info.created  = CTime::Now();
    evt->info.ticked   = 0;
    evt->info.at       = at;

    // add to event loop at first interval
    timeval first;
    first.tv_sec  = interval.id;
    first.tv_usec = 0;
    evtimer_add(evt->lbevent, &first);
    mEvents[evt->info.id] = evt;

    MONITOR(OnTimerAddOrDelete(true));
    LOG_INFO("AddTimer{ id=%u, type=%u, loops=%u, first=%u, normal=%u, at[%u:%u:%u] }",
            evt->info.id, evt->info.type, evt->info.loops, evt->info.interval.id,
            evt->info.interval.value, evt->info.at.type, evt->info.at.id, evt->info.at.value);

    return evt->info.id;
}
#undef CHECK_INTERVAL
#undef CHECK_WEEKDAY
#undef CHECK_HOUR
#undef CHECK_MINUTE
#undef CHECK_SECOND

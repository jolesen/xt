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
    m_seq = 0;
}

CTimer::~CTimer()
{
}

uint CTimer::AddSecond(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickSecond, S2UInt(SECOND * interval, SECOND * interval), loops, S3UInt(), cb)->info.id;
}

uint CTimer::AddMinute(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickMinute, S2UInt(MINUTE * interval, MINUTE * interval), loops, S3UInt(), cb)->info.id;
}

uint CTimer::AddHour(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickHour, S2UInt(HOUR * interval, HOUR * interval), loops, S3UInt(), cb)->info.id;
}

uint CTimer::AddDay(uint interval, uint loops, FunTimeout cb)
{
    CHECK_INTERVAL;
    return CreateTick(ETickDay, S2UInt(DAY * interval, DAY * interval), loops, S3UInt(), cb)->info.id;
}

uint CTimer::AddMinuteAt(uint interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_SECOND(at.type);

    STime now = CTime::Parse(CTime::Now());

    uint now_second = now.second;
    uint at_second  = at.type;

    S2UInt s(0, (MINUTE * interval));
    if(at_second >= now_second)
    {
        s.id = at_second - now_second;
    }
    else
    {
        s.id = (MINUTE - now_second) + ((interval - 1) * MINUTE) + at_second;
    }

    return CreateTick(ETickMinuteAt, s, loops, at, cb)->info.id;
}

uint CTimer::AddHourAt(uint interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_MINUTE(at.type);
    CHECK_SECOND(at.id);

    STime now = CTime::Parse(CTime::Now());

    uint now_second = (now.minute * MINUTE) + now.second;
    uint at_second  = (at.type    * MINUTE) + at.id;

    S2UInt s(0, (HOUR * interval));
    if(at_second >= now_second)
    {
        s.id = (at_second - now_second);
    }
    else
    {
        s.id = (HOUR - now_second) + ((interval - 1) * HOUR) + at_second;
    }

    return CreateTick(ETickHourAt, s, loops, at, cb)->info.id;
}

uint CTimer::AddDayAt(uint interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_HOUR(at.type);
    CHECK_MINUTE(at.id);
    CHECK_SECOND(at.value);

    STime now = CTime::Parse(CTime::Now());

    uint now_second = (now.hour * HOUR) + (now.minute * MINUTE) + now.second;
    uint at_second  = (at.type  * HOUR) + (at.id      * MINUTE) + at.value;

    S2UInt s(0, (DAY * interval));
    if(at_second >= now_second)
    {
        s.id = (at_second - now_second);
    }
    else
    {
        s.id = (DAY - now_second) + ((interval - 1) * DAY) + at_second;
    }

    return CreateTick(ETickDayAt, s, loops, at, cb)->info.id;
}

uint CTimer::AddWeekDayAt(uint interval, uint loops, EWeekDay day, const S3UInt &at, FunTimeout cb)
{
    CHECK_INTERVAL;
    CHECK_WEEKDAY(day);
    CHECK_HOUR(at.type);
    CHECK_MINUTE(at.id);
    CHECK_SECOND(at.value);

    STime now = CTime::Parse(CTime::Now());

    uint now_wday = now.wday;
    uint at_wday  = (uint)day;

    uint now_second = (now_wday * DAY) + (now.hour * HOUR) + (now.minute * MINUTE) + now.second;
    uint at_second  = (at_wday  * DAY) + (at.type  * HOUR) + (at.id      * MINUTE) + at.value;

    S2UInt s(0, (WEEK * interval));
    if(at_second >= now_second)
    {
        s.id = (at_second - now_second);
    }
    else
    {
        s.id = (WEEK - now_second) + ((interval - 1) * WEEK) + at_second;
    }

    return CreateTick(ETickWeekDayAt, s, loops, at, cb)->info.id;
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
        event_assign(evt->lbevent, theTimer.m_base, -1, EV_PERSIST, CTimer::OnTimeout, evt);
        evtimer_add(evt->lbevent, &normal);
        LOG_DEBUG("Convert timer=%u to be persist.", evt->info.id);
    }
}

void CTimer::Delete(uint id)
{
    IF_NFIND(m_events, id, iter)
    {
        LOG_ERROR("Can't find event, id=%u", id);
        return;
    }

    STimerEvent *evt = iter->second;
    evtimer_del(evt->lbevent);
    m_events.erase(evt->info.id);
    delete evt;
    LOG_DEBUG("Delete timer:%u", id);

    MONITOR(OnTimerAddOrDelete(false));
}

STimerEvent* CTimer::CreateTick(ETick type, const S2UInt &interval, uint loops, const S3UInt &at, FunTimeout cb)
{
    STimerEvent *evt = new STimerEvent;
    if(at.type > 0) // is tick at
    {
        evt->lbevent = evtimer_new(m_base, CTimer::OnTimeout, evt);
    }
    else
    {
        evt->lbevent = event_new(m_base, -1, EV_PERSIST, CTimer::OnTimeout, evt);
    }
    evt->callback = cb;

    evt->info.id       = (++m_seq);
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
    m_events[evt->info.id] = evt;

    MONITOR(OnTimerAddOrDelete(true));
    LOG_INFO("AddTimer{ id=%u, type=%u, loops=%u, first=%u, normal=%u, at[%u:%u:%u] }",
            evt->info.id, evt->info.type, evt->info.loops, evt->info.interval.id,
            evt->info.interval.value, evt->info.at.type, evt->info.at.id, evt->info.at.value);

    return evt;
}
#undef CHECK_INTERVAL
#undef CHECK_WEEKDAY
#undef CHECK_HOUR
#undef CHECK_MINUTE
#undef CHECK_SECOND

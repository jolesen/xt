#ifndef _COMMON_CORE_TIMER_H_
#define _COMMON_CORE_TIMER_H_

#include <event2/event.h>
#include "core.h"

// ETick
enum ETick
{
    ETickSecond = 1, ETickMinute, ETickMinuteAt, ETickHour, ETickHourAt, ETickDay, ETickDayAt, ETickWeekDayAt
};

// STimerInfo
struct STimerInfo
{
    uint    id;
    ETick   type;
    uint    loops;
    S2UInt  interval; // .id : first interval, .value : others' interval
    uint    created;
    uint    ticked; 
    S3UInt  at;
};

// FunTimeout, if reutrn false, means to stop this timer
typedef bool (*FunTimeout)(const STimerInfo &info);

// STimerEvent
struct STimerEvent
{
    STimerInfo info;
    event*     lbevent;
    FunTimeout callback;
};

// CTimer
class CTimer
{
public:
    void Init(event_base* base) { m_base = base; }
    void Delete(uint id);

    // loops is 0 if infinite, return timer's id
    uint AddSecond(uint interval, uint loops, FunTimeout cb);
    uint AddMinute(uint interval, uint loops, FunTimeout cb);
    uint AddHour  (uint interval, uint loops, FunTimeout cb);
    uint AddDay   (uint interval, uint loops, FunTimeout cb);

    uint AddMinuteAt (uint interval, uint loops, const S3UInt &at, FunTimeout cb); // at.type need
    uint AddHourAt   (uint interval, uint loops, const S3UInt &at, FunTimeout cb); // at.type and at.id need
    uint AddDayAt    (uint interval, uint loops, const S3UInt &at, FunTimeout cb); // at.* need
    uint AddWeekDayAt(uint interval, uint loops, EWeekDay day, const S3UInt &at, FunTimeout cb); // at.* need

private:
    static void OnTimeout(evutil_socket_t fd, short event, void *args);
    uint CreateTick(ETick type, const S2UInt &interval, uint loops, const S3UInt &at, FunTimeout cb);

private:
    friend CTimer& CSingleton<CTimer>::Instance();
    CTimer();
    ~CTimer();

    event_base *m_base;
    uint        m_seq;
    std::map<uint, STimerEvent*> m_events;
};
#define theTimer CSingleton<CTimer>::Instance()

#endif

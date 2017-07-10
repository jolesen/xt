#ifndef _COMMON_CORE_TIME_H_
#define _COMMON_CORE_TIME_H_

#include <time.h>
#include "core.h"

struct STime
{
    uint year;
    uint month;
    uint wday;
    uint mday;
    uint yday;
    uint hour;
    uint minute;
    uint second;
};

// Format types: "2010-10-10 10:10:10", "2010-10-10", "10:10:10"
class CTime
{
public:
    static uint Now() { return time(NULL); }
    static STime Parse(uint time);

    // string to uint
    static uint ParseDateTime(const string &strTime);
    static uint ParseDate(const string &strDate);
    static uint ConcatDateTime(const string &strDate, const string &strTime) { return ParseDateTime(strDate + " " + strTime); }

    // uint to string
    static string FormatDateTime(uint time);
    static string FormatTime(uint time);
    static string FormatDate(uint time);
    static string FormatCurrentDateTime() { return FormatDateTime(Now()); }
    static string FormatCurrentTime() { return FormatTime(Now()); }
    static string FormatCurrentDate() { return FormatDate(Now()); }

    // compare
    static bool IsSameMonth(uint a, uint b);
    static bool IsSameHour(uint a, uint b);
    static bool IsSameDay(uint a, uint b);
    static bool IsLeapYear(uint t);
    static bool IsToday(uint t) { return IsSameDay(t, Now()); }

    // getter
    static uint GetSubDay(uint timeA, uint timeB);
    static uint GetMonthMaxDay(uint year, uint month);
    static uint GetZeroTime(uint time);
    static uint GetYearMaxDay(uint year) { return IS_LEAP_YEAR(year) ? 366 : 365; }
    static uint GetCurrentZeroTime() { return GetZeroTime(Now()); }
    static uint GetDayLeftSeconds(uint time) { return DAY - (time - GetZeroTime(time)); }

private:
    CTime();
    ~CTime();
};

#endif

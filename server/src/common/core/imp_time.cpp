#include "time.h"

CTime::CTime()
{
}

CTime::~CTime()
{
}

STime CTime::Parse(uint time)
{
    //struct tm
    //{
    //    int tm_sec;   // 秒 – 取值区间为[0,59]
    //    int tm_min;   // 分 - 取值区间为[0,59]
    //    int tm_hour;  // 时 - 取值区间为[0,23] 
    //    int tm_mday;  // 一个月中的日期 - 取值区间为[1,31]
    //    int tm_mon;   // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
    //    int tm_year;  // 年份，其值等于实际年份减去1900 
    //    int tm_wday;  // 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 
    //    int tm_yday;  // 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 
    //    int tm_isdst; // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
    //};

    struct tm tmp;
    time_t t = time;
    localtime_r(&t, &tmp);

    STime result;
    result.year   = tmp.tm_year + 1900;
    result.month  = tmp.tm_mon  + 1;
    result.wday   = tmp.tm_wday;
    result.mday   = tmp.tm_mday;
    result.yday   = tmp.tm_yday;
    result.hour   = tmp.tm_hour;
    result.minute = tmp.tm_min;
    result.second = tmp.tm_sec;

    return result;
}

uint CTime::ParseDateTime(const string &strTime)
{
    struct tm t = { 0 };
    if(sscanf(strTime.c_str(), "%u-%u-%u %u:%u:%u", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) == 6)
    {
        t.tm_year -= 1900;
        t.tm_mon  -= 1;
        return (uint)mktime(&t);
    }

    return 0;
}

uint CTime::ParseDate(const string &strDate)
{
    struct tm t = { 0 };
    if(sscanf(strDate.c_str(), "%u-%u-%u", &t.tm_year, &t.tm_mon, &t.tm_mday) == 3)
    {
        t.tm_year -= 1900;
        t.tm_mon  -= 1;
        return (uint)mktime(&t);
    }

    return 0;
}

string CTime::FormatTime(uint time)
{
    STime st = Parse(time);
    char result[32] = { 0 };
    snprintf(result, sizeof(result), "%02u:%02u:%02u", st.hour, st.minute, st.second);

    return result;
}

string CTime::FormatDate(uint time)
{
    STime st = Parse(time);
    char result[32] = { 0 };
    snprintf(result, sizeof(result), "%04u-%02u-%02u", st.year, st.month, st.mday);

    return result;
}

string CTime::FormatDateTime(uint time)
{
    STime st = Parse(time);
    char result[64] = { 0 };
    snprintf(result, sizeof(result), "%04u-%02u-%02u %02u:%02u:%02u", st.year, st.month, st.mday, st.hour, st.minute, st.second);

    return result;
}

bool CTime::IsSameHour(uint a, uint b)
{
    STime ta = Parse(a);
    STime tb = Parse(b);

    return (ta.year == tb.year) && (ta.yday == tb.yday) && (ta.hour == tb.hour);
}

bool CTime::IsSameDay(uint a, uint b)
{
    STime ta = Parse(a);
    STime tb = Parse(b);

    return (ta.year == tb.year) && (ta.yday == tb.yday);
}

bool CTime::IsSameMonth(uint a, uint b)
{
    STime ta = Parse(a);
    STime tb = Parse(b);

    return (ta.year == tb.year) && (ta.month == tb.month);
}

bool CTime::IsLeapYear(uint time)
{
    STime t = Parse(time);
    return IS_LEAP_YEAR(t.year);
}

uint CTime::GetSubDay(uint timeA, uint timeB)
{
    if(timeB >= timeA)
    {
        uint zero_a = GetZeroTime(timeA);
        uint zero_b = GetZeroTime(timeB);
        return (zero_b - zero_a) / DAY;
    }

    return 0;
}

uint CTime::GetMonthMaxDay(uint year, uint month)
{
    switch(month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
    }

    // february
    return IsLeapYear(year) ? 29 : 28;
}

uint CTime::GetZeroTime(uint time)
{
    struct tm t;
    localtime_r((time_t*)&time, &t);
    t.tm_hour = 0;
    t.tm_min  = 0;
    t.tm_sec  = 0;

    return (uint)mktime(&t);
}

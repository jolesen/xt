#ifndef _COMMON_CORE_LOG_H_
#define _COMMON_CORE_LOG_H_

#include "mutex.h"

// CLog
class CLog
{
public:
    void Log(const char* src, const char* func, uint line, const char* fmt, ...);
    void SetFileName(const std::string &name) { mFilename = name; }

private:
    friend CLog& CSingleton<CLog>::Instance();
    CLog();
    ~CLog();

private:
    std::string mFilename;
};
#define theLog CSingleton<CLog>::Instance()

// -------------- LOG MACROS--------------
#ifdef DEBUG // DEBUG macro defined by para of cmake.
    #define LOG_DEBUG(fmt, ...) theLog.Log(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...)
#endif
#define LOG_INFO(fmt,  ...) theLog.Log(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt,  ...) theLog.Log(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) theLog.Log(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif

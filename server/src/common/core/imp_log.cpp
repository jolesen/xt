#include <sys/timeb.h>
#include <stdarg.h>
#include "time.h"
#include "log.h"

CLog::CLog()
{
}

CLog::~CLog()
{
}

void CLog::Log(const char* src, const char* func, uint line, const char* fmt, ...)
{
    // OPEN
    FILE *file = fopen(mFilename.c_str(), "a+");
    if(!file)
    {
        return;
    }

    // DATE
    char strDate[128] = { 0 };
    struct timeb tb;
    ftime(&tb);
    STime st = CTime::Parse(tb.time);
    snprintf(strDate, sizeof(strDate), "[%04u-%02u-%02u %02u:%02u:%02u,%03u]", st.year, st.month, st.mday, st.hour, st.minute, st.second, tb.millitm);

    // FILE
    char strFfl[64]     = { 0 };
    std::string strFile = src;
    std::size_t pos     = strFile.find_last_of("/");
    const char* sfile   = (pos != std::string::npos) ? strFile.substr(pos + 1).c_str() : strFile.c_str();
    snprintf(strFfl, sizeof(strFfl), "[%s][%s:%u]", sfile, func, line);

    // CONTENT
    char strFmt[KB / 2] = { 0 };
    va_list args;
    va_start(args, fmt);
    vsnprintf(strFmt, sizeof(strFmt), fmt, args);
    va_end(args);

    // WRITE
    fprintf(file, "%s%s: %s\n", strDate, strFfl, strFmt);
    fclose(file);
}

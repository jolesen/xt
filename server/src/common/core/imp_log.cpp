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
    m_mutex.Lock();
    {
        // OPEN
        FILE *file = fopen(m_filename.c_str(), "a+");
        if(!file)
        {
            m_mutex.Unlock(); // unlock
            return;
        }

        // DATE
        char str_date[128] = { 0 };
        struct timeb tb;
        ftime(&tb);
        STime st = CTime::Parse(tb.time);
        snprintf(str_date, sizeof(str_date), "[%04u-%02u-%02u %02u:%02u:%02u,%03u]", st.year, st.month, st.mday, st.hour, st.minute, st.second, tb.millitm);

        // FILE
        char str_ffl[64]     = { 0 };
        std::string str_file = src;
        std::size_t pos      = str_file.find_last_of("/");
        const char* sfile    = (pos != std::string::npos) ? str_file.substr(pos + 1).c_str() : str_file.c_str();
        snprintf(str_ffl, sizeof(str_ffl), "[%s][%s:%u]", sfile, func, line);

        // CONTENT
        char str_fmt[KB / 2] = { 0 };
        va_list args;
        va_start(args, fmt);
        vsnprintf(str_fmt, sizeof(str_fmt), fmt, args);
        va_end(args);

        // WRITE
        fprintf(file, "%s%s: %s\n", str_date, str_ffl, str_fmt);
        fclose(file);
    }
    m_mutex.Unlock();
}

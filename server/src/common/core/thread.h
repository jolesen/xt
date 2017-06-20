#ifndef _COMMON_CORE_THREAD_H_
#define _COMMON_CORE_THREAD_H_

#include <pthread.h>
#include "core.h"

// CThread
class CThread
{
public:
    CThread();
    ~CThread();

    void Start();
    void Stop();

    virtual void Run() = 0;

protected:
    pthread_attr_t m_attr;
    pthread_t      m_thread;

private:
    bool m_started;
};

#endif

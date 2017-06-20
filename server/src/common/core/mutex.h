#ifndef _COMMON_CORE_MUTEX_H_
#define _COMMON_CORE_MUTEX_H_

#include "core.h"

// CMutex
class CMutex
{
public:
    CMutex()  { Create();  }
    ~CMutex() { Destory(); }

    bool Lock()    { return (pthread_mutex_lock(&m_mutex)    == 0); }
    bool TryLock() { return (pthread_mutex_trylock(&m_mutex) == 0); }
    bool Unlock()  { return (pthread_mutex_unlock(&m_mutex)  == 0); }
    bool Destory() { return (pthread_mutex_destroy(&m_mutex) == 0); }

private:
    void Create()  { pthread_mutex_init(&m_mutex, NULL); }

private:
    pthread_mutex_t m_mutex;
};

// CMutexSafe
class CMutexSafe
{
public:
    CMutexSafe(CMutex *mutex)
    {
        m_mutex = mutex;
        if(m_mutex)
        {
            m_mutex->Lock();
        }
    }

    ~CMutexSafe()
    {
        if(m_mutex)
        {
            m_mutex->Unlock();
        }
        m_mutex = NULL;
    }

private:
    CMutex* m_mutex;
};

#endif

#ifndef _COMMON_CORE_MUTEX_H_
#define _COMMON_CORE_MUTEX_H_

#include "core.h"

// CMutex
class CMutex
{
public:
    CMutex()  { Create();  }
    ~CMutex() { Destory(); }

    bool Lock()    { return (pthread_mutex_lock(&mMutex)    == 0); }
    bool TryLock() { return (pthread_mutex_trylock(&mMutex) == 0); }
    bool Unlock()  { return (pthread_mutex_unlock(&mMutex)  == 0); }
    bool Destory() { return (pthread_mutex_destroy(&mMutex) == 0); }

private:
    void Create()  { pthread_mutex_init(&mMutex, NULL); }

private:
    pthread_mutex_t mMutex;
};

// CMutexSafe
class CMutexSafe
{
public:
    CMutexSafe(CMutex *mutex)
    {
        mMutex = mutex;
        if(mMutex)
        {
            mMutex->Lock();
        }
    }

    ~CMutexSafe()
    {
        if(mMutex)
        {
            mMutex->Unlock();
        }
        mMutex = NULL;
    }

private:
    CMutex* mMutex;
};

#endif

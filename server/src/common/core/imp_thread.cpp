#include "thread.h"

// CThread
CThread::CThread()
{
    mStarted = false;
}

CThread::~CThread()
{
    Stop();
}

void* _RunThread(void *thread)
{
    if(thread)
    {
        CThread *th = (CThread*)thread;
        if(th)
        {
            th->Run();
        }
    }

    return NULL;
}

void CThread::Start()
{
    if(!mStarted)
    {
        pthread_attr_init(&mAttr);
        pthread_attr_setscope(&mAttr, PTHREAD_SCOPE_SYSTEM);
        pthread_attr_setdetachstate(&mAttr, PTHREAD_CREATE_JOINABLE);

        pthread_create(&mThread, &mAttr, _RunThread, (void*)this);

        mStarted = true;
    }
}

void CThread::Stop()
{
    if(mStarted)
    {
        pthread_join(mThread, NULL);
    }

    mStarted = false;
}

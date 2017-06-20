#include "thread.h"

// CThread
CThread::CThread()
{
    m_started = false;
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
    if(!m_started)
    {
        pthread_attr_init(&m_attr);
        pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM);
        pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_JOINABLE);

        pthread_create(&m_thread, &m_attr, _RunThread, (void*)this);

        m_started = true;
    }
}

void CThread::Stop()
{
    if(m_started)
    {
        pthread_join(m_thread, NULL);
    }

    m_started = false;
}

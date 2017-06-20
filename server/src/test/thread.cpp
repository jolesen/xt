#include "common/core/util.h"
#include "test.h"

void TestThread()
{
    CTestThread thread;
    thread.Start();
    for(;;)
    {
    }
}

CTestThread::CTestThread()
{
}

CTestThread::~CTestThread()
{
}

void CTestThread::Run()
{
    for(; ;)
    {
        uint static count = 0;
        printf("count: %u\n", count++);
        fflush(stdout);

        CUtil::SleepMillisecond(1 * 1000);
    }
}

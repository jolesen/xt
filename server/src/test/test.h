#ifndef _TEST_TEST_H_
#define _TEST_TEST_H_

#include "common/core/thread.h"

void TestCoder();
void TestThread();

class CTestThread : public CThread
{
public:
    CTestThread();
    ~CTestThread();

    void Run();
};

#endif

#ifndef _COMMON_CORE_MONITOR_H_
#define _COMMON_CORE_MONITOR_H_

#include "thread.h"

// CMonitorThread
class CMonitor;
class CMonitorThread : public CThread
{
public:
    friend class CMonitor;
    CMonitorThread();
    ~CMonitorThread();

public:
    void Run();
};

// CMonitor
class CMonitor
{
public:
    struct SMsgInfo
    {
        UIntUIntMap  count;
        UIntUIntMap  max;
        UIntULongMap total;
    };

public:
    void Start();
    void AddLinkState(const string &sid, uint state, uint reason);
    bool IsDirty() { return mDirty; }
    void SetTidyUp() { mDirty = false; }

    void OnReadMsg(uint id, uint size);
    void OnWriteMsg(uint id, uint size);
    void OnTimerTicked();
    void OnTimerAddOrDelete(bool isAdd);
    void OnOpenFd(uint fd);
    void OnHttpRequest(const string &uri);
    void OnClientCreated();
    void OnClientPoolChanged(uint size);

public:
    typedef std::map<string, S3UIntList> LinkStateMap; // type - 时间，id - 状态，value - 原因

    LinkStateMap GetLinkState() { return mState; }
    SMsgInfo GetMsgRead() { return mMsgRead; }
    SMsgInfo GetMsgWrite() { return mMsgWrite; }
    S2UInt GetTimerInfo() { return mTimer; }
    uint GetMaxOpenFd() { return mMaxFd; }
    uint GetHttpRequestCount() { return mHttpCount; }
    uint GetClientCreatedCount() { return mClientCreated; }
    uint GetClientPoolSize() { return mClientPool; }

private:
    friend CMonitor& CSingleton<CMonitor>::Instance();
    CMonitor();
    ~CMonitor();

private:
    CMonitorThread mThread;
    LinkStateMap   mState;
    SMsgInfo       mMsgRead;
    SMsgInfo       mMsgWrite;
    S2UInt         mTimer; // id - count, value - ticked;
    uint           mMaxFd;
    uint           mHttpCount;
    uint           mClientCreated;
    uint           mClientPool;
    bool           mDirty;
};
#define theMonitor CSingleton<CMonitor>::Instance()

// 调用监视器
#ifdef USE_MONITOR
    #define MONITOR(CALL) theMonitor.CALL
#else
    #define MONITOR(CALL) 
#endif

#endif

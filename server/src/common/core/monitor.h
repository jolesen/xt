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
    void AddLinkState(const std::string &sid, uint state, uint reason);
    bool IsDirty() { return m_dirty; }
    void SetTidyUp() { m_dirty = false; }

    void OnReadMsg(uint id, uint size);
    void OnWriteMsg(uint id, uint size);
    void OnTimerTicked();
    void OnTimerAddOrDelete(bool is_add);
    void OnOpenFd(uint fd);
    void OnHttpRequest(const std::string &uri);
    void OnClientCreated();
    void OnClientPoolChanged(uint size);

public:
    typedef std::map<std::string, S3UIntList> LinkStateMap; // type - 时间，id - 状态，value - 原因

    LinkStateMap GetLinkState() { return m_state; }
    SMsgInfo GetMsgRead() { return m_msg_read; }
    SMsgInfo GetMsgWrite() { return m_msg_write; }
    S2UInt GetTimerInfo() { return m_timer; }
    uint GetMaxOpenFd() { return m_max_fd; }
    uint GetHttpRequestCount() { return m_http_count; }
    uint GetClientCreatedCount() { return m_client_created; }
    uint GetClientPoolSize() { return m_client_pool; }

private:
    friend CMonitor& CSingleton<CMonitor>::Instance();
    CMonitor();
    ~CMonitor();

private:
    CMonitorThread m_thread;
    LinkStateMap   m_state;
    SMsgInfo       m_msg_read;
    SMsgInfo       m_msg_write;
    S2UInt         m_timer; // id - count, value - ticked;
    uint           m_max_fd;
    uint           m_http_count;
    uint           m_client_created;
    uint           m_client_pool;
    bool           m_dirty;
};
#define theMonitor CSingleton<CMonitor>::Instance()

// 调用监视器
#ifdef USE_MONITOR
    #define MONITOR(CALL) theMonitor.CALL
#else
    #define MONITOR(CALL) 
#endif

#endif

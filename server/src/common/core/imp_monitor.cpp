#include "monitor.h"
#include "sconfig.h"
#include "server.h"
#include "linker.h"
#include "timer.h"
#include "time.h"

// 是否开启
#define CHECK_OPEN()   if(!theServerConfig.monitor_tick) { return; }

// CMonitor
CMonitor::CMonitor()
{
    m_max_fd         = 0;
    m_http_count     = 0;
    m_client_created = 0;
    m_client_pool    = 0;
    m_dirty          = false;
}

CMonitor::~CMonitor()
{
    m_thread.Stop();
}

void CMonitor::AddLinkState(const std::string &sid, uint state, uint reason)
{
    CHECK_OPEN();

    S3UIntList &list = m_state[sid];
    if(list.size())
    {
        S3UInt &last = list[list.size() - 1];
        if((last.id == state) && (last.value == reason))
        {
            return;
        }
    }

    S3UInt st;
    st.type  = CTime::Now();
    st.id    = state;
    st.value = reason;
    m_state[sid].push_back(st);

    m_dirty = true;
}

void CMonitor::Start()
{
    CHECK_OPEN();

    m_thread.Start();
}

void _OnMsg(CMonitor::SMsgInfo &info, uint id, uint size)
{
    info.count[id] += 1;
    info.max[id]    = MAX(size, info.max[id]);
    info.total[id] += size;
}

void CMonitor::OnReadMsg(uint id, uint size)
{
    CHECK_OPEN();

    _OnMsg(m_msg_read, id, size);
    m_dirty = true;
}

void CMonitor::OnWriteMsg(uint id, uint size)
{
    CHECK_OPEN();

    _OnMsg(m_msg_write, id, size);
    m_dirty = true;
}

void CMonitor::OnTimerTicked()
{
    CHECK_OPEN();

    ++m_timer.value;
    m_dirty = true;
}

void CMonitor::OnTimerAddOrDelete(bool is_add)
{
    CHECK_OPEN();

    m_timer.id = is_add ? (m_timer.id + 1) : (m_timer.id - 1);
    m_dirty = true;
}

void CMonitor::OnOpenFd(uint fd)
{
    CHECK_OPEN();

    if(fd > m_max_fd)
    {
        m_max_fd = fd;
        m_dirty = true;
    }
}

void CMonitor::OnHttpRequest(const std::string &uri)
{
    CHECK_OPEN();

    ++m_http_count;
    m_dirty = true;
}

void CMonitor::OnClientCreated()
{
    CHECK_OPEN();

    ++m_client_created;
    m_dirty = true;
}

void CMonitor::OnClientPoolChanged(uint size)
{
    CHECK_OPEN();

    m_client_pool = size;
    m_dirty = true;
}

// CMonitorThread
CMonitorThread::CMonitorThread()
{
}

CMonitorThread::~CMonitorThread()
{
}

void _WriteMsgInfo(FILE *file, bool is_read)
{
    // msg count
    CMonitor::SMsgInfo info = is_read ? theMonitor.GetMsgRead() : theMonitor.GetMsgWrite();
    uint count_total = 0;
    S2UInt count_max;
    FORMAP(info.count, iter)
    {
        count_total += iter->second;
        if(iter->second > count_max.value)
        {
            count_max.id    = iter->first;
            count_max.value = iter->second;
        }
    }
    std::string count_max_name = theServerConfig.msg_name_getter(count_max.id);

    // msg size
    uint size_max   = 0;
    uint size_msgId = 0;
    FORMAP(info.max, iter)
    {
        if(iter->second > size_max)
        {
            size_max   = iter->second;
            size_msgId = iter->first;
        }
    }
    std::string size_max_name = theServerConfig.msg_name_getter(size_msgId);

    // msg total bytes
    ulong total = 0;
    ulong total_msg_max = 0;
    uint total_msg_id = 0;
    FORMAP(info.total, iter)
    {
        total += iter->second;
        if(iter->second > total_msg_max)
        {
            total_msg_max = iter->second;
            total_msg_id  = iter->first;
        }
    }
    std::string total_max_name = theServerConfig.msg_name_getter(total_msg_id);

    std::string strFlag = is_read ? "READ" : "WRITE";
    fprintf(file, "MSG %-5s COUNT[%u, %s: %u], MAXSIZE[%s: %uB], TOTAL[%juB, %s: %juB]\n", strFlag.c_str(),
            count_total, count_max_name.c_str(), count_max.value, size_max_name.c_str(), size_max, total, total_max_name.c_str(), total_msg_max);
}

void _WriteDetail(FILE *file)
{
    S2UInt timer = theMonitor.GetTimerInfo();
    fprintf(file, "SERVER STARTED TIME: %s\n", CTime::FormatDateTime(theServer.GetStartTime()).c_str());
    fprintf(file, "SERVER MAX FD INDEX: %u\n", theMonitor.GetMaxOpenFd());
    _WriteMsgInfo(file, true);
    _WriteMsgInfo(file, false);
    fprintf(file, "CLIENT ACCEPT/LINK: %u/%u\n", theServer.GetClientCount(), theLinker.GetLinkCount());
    fprintf(file, "CLIENT POOL SIZE/CREATED: %u/%u\n", theMonitor.GetClientPoolSize(), theMonitor.GetClientCreatedCount());
    fprintf(file, "TIMER COUNT/TICKED: %u/%u\n", timer.id, timer.value);
    if(theServerConfig.http_host.port)
    {
        fprintf(file, "HTTP REQUEST COUNT: %d\n", theMonitor.GetHttpRequestCount());
    }
    fprintf(file, "\n");

    CMonitor::LinkStateMap mpState = theMonitor.GetLinkState();
    FORMAP(mpState, iter)
    {
        fprintf(file, "LINK -> %7s: ", iter->first.c_str());
        FORLIST(iter->second, i)
        {
            const S3UInt &st = (iter->second)[i];
            if(st.value)
            {
                fprintf(file, "[%u:%u,%u] ", st.type, st.id, st.value);
            }
            else
            {
                fprintf(file, "[%u:%u] ", st.type, st.id);
            }
        }
        fprintf(file, "\n");
    }
}

void CMonitorThread::Run()
{
    char strFile[128] = { 0 };
    snprintf(strFile, sizeof(strFile), "%s.monitor", theServerConfig.sid.c_str());

    for(;;)
    {
        do
        {
            if(!theMonitor.IsDirty())
            {
                break;
            }

            FILE *file = fopen(strFile, "w+");
            if(!file)
            {
                break;
            }

            _WriteDetail(file);

            fclose(file);

            theMonitor.SetTidyUp();
        }
        while(false);

        // sleep
        CUtil::SleepSecond(theServerConfig.monitor_tick);
    }
}

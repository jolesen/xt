#include "monitor.h"
#include "sconfig.h"
#include "server.h"
#include "linker.h"
#include "timer.h"
#include "time.h"

// 是否开启
#define CHECK_OPEN()   if(!theServerConfig.monitorTick) { return; }

// CMonitor
CMonitor::CMonitor()
{
    mMaxFd         = 0;
    mHttpCount     = 0;
    mClientCreated = 0;
    mClientPool    = 0;
    mDirty         = false;
}

CMonitor::~CMonitor()
{
    mThread.Stop();
}

void CMonitor::AddLinkState(const std::string &sid, uint state, uint reason)
{
    CHECK_OPEN();

    S3UIntList &list = mState[sid];
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
    mState[sid].push_back(st);

    mDirty = true;
}

void CMonitor::Start()
{
    CHECK_OPEN();

    mThread.Start();
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

    _OnMsg(mMsgRead, id, size);
    mDirty = true;
}

void CMonitor::OnWriteMsg(uint id, uint size)
{
    CHECK_OPEN();

    _OnMsg(mMsgWrite, id, size);
    mDirty = true;
}

void CMonitor::OnTimerTicked()
{
    CHECK_OPEN();

    ++mTimer.value;
    mDirty = true;
}

void CMonitor::OnTimerAddOrDelete(bool isAdd)
{
    CHECK_OPEN();

    mTimer.id = isAdd ? (mTimer.id + 1) : (mTimer.id - 1);
    mDirty = true;
}

void CMonitor::OnOpenFd(uint fd)
{
    CHECK_OPEN();

    if(fd > mMaxFd)
    {
        mMaxFd = fd;
        mDirty = true;
    }
}

void CMonitor::OnHttpRequest(const std::string &uri)
{
    CHECK_OPEN();

    ++mHttpCount;
    mDirty = true;
}

void CMonitor::OnClientCreated()
{
    CHECK_OPEN();

    ++mClientCreated;
    mDirty = true;
}

void CMonitor::OnClientPoolChanged(uint size)
{
    CHECK_OPEN();

    mClientPool = size;
    mDirty = true;
}

// CMonitorThread
CMonitorThread::CMonitorThread()
{
}

CMonitorThread::~CMonitorThread()
{
}

void _WriteMsgInfo(FILE *file, bool isRead)
{
    // msg count
    CMonitor::SMsgInfo info = isRead ? theMonitor.GetMsgRead() : theMonitor.GetMsgWrite();
    uint countTotal = 0;
    S2UInt countMax;
    FORMAP(info.count, iter)
    {
        countTotal += iter->second;
        if(iter->second > countMax.value)
        {
            countMax.id    = iter->first;
            countMax.value = iter->second;
        }
    }
    std::string countMaxName = theServerConfig.funGetMsgName(countMax.id);

    // msg size
    uint sizeMax   = 0;
    uint sizeMsgId = 0;
    FORMAP(info.max, iter)
    {
        if(iter->second > sizeMax)
        {
            sizeMax   = iter->second;
            sizeMsgId = iter->first;
        }
    }
    std::string sizeMaxName = theServerConfig.funGetMsgName(sizeMsgId);

    // msg total bytes
    ulong total = 0;
    ulong totalMsgMax = 0;
    uint totalMsgId = 0;
    FORMAP(info.total, iter)
    {
        total += iter->second;
        if(iter->second > totalMsgMax)
        {
            totalMsgMax = iter->second;
            totalMsgId  = iter->first;
        }
    }
    std::string totalMaxName = theServerConfig.funGetMsgName(totalMsgId);

    std::string strFlag = isRead ? "READ" : "WRITE";
    fprintf(file, "MSG %-5s COUNT[%u, %s: %u], MAXSIZE[%s: %uB], TOTAL[%juB, %s: %juB]\n", strFlag.c_str(),
            countTotal, countMaxName.c_str(), countMax.value, sizeMaxName.c_str(), sizeMax, total, totalMaxName.c_str(), totalMsgMax);
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
    if(theServerConfig.httpHost.port)
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
        CUtil::SleepSecond(theServerConfig.monitorTick);
    }
}

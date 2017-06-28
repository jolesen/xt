#include <event2/buffer.h>
#include <cstring>
#include "errorcode.h"
#include "sconfig.h"
#include "monitor.h"
#include "client.h"

CClient::CClient()
{
    SHost host;
    Init(host, NULL, "");
}

CClient::~CClient()
{
}

void CClient::SetPassive(const SHost &host, bufferevent *bev)
{
    Init(host, bev, "");
}

void CClient::SetActive(const SHost &host, const std::string &sid)
{
    Init(host, NULL, sid);
}

void CClient::Init(const SHost &host, bufferevent *bev, const std::string &sid)
{
    mHost  = host;
    mId    = 0;
    mSid   = sid;
    mBev   = bev;
    mState = bev ? kClientStateConnected : kClientStateClosed;
    mType  = bev ? kClientTypePassive    : kClientTypeActive;
}

uint CClient::Connect(event_base *base, FunConnected funConnect)
{
    if(mType != kClientTypeActive)
    {
        return ERR_CORE_CLINET_CONNECT_TYPE;
    }
    if(mBev)
    {
        return ERR_CORE_CLINET_CONNECT_WITH_BUFFER;
    }

    // addr
    sockaddr_in addr;
    memset((void*)&addr, 0, sizeof(sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = CUtil::GetIpInt(mHost.ip);
    addr.sin_port        = htons(mHost.port);

    // connect
    mState = kClientStateConnecting;
    mBev   = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(mBev, NULL, NULL, funConnect, NULL);
    bufferevent_socket_connect(mBev, (sockaddr*)&addr, sizeof(addr));

    MONITOR(OnOpenFd(GetFd()));

    return 0;
}

uint CClient::Read(char *buff, uint buffSize, uint &readSize)
{
    readSize = 0; // important

    if(!buff)
    {
        return ERR_CORE_CLINET_READ_PARAM;
    }
    if(!mBev)
    {
        return ERR_CORE_CLINET_READ_NO_BUFF;
    }

    // copy
    evbuffer *input = bufferevent_get_input(mBev);
    int copy = evbuffer_copyout(input, buff, buffSize);
    if(copy < 0) // copy error
    {
        return ERR_CORE_CLINET_READ_BUFF_COPY;
    }
    if(copy == 0) // no packet
    {
        return 0;
    }
    if((uint)copy < SMsgHeader::GetMinSize()) // min size
    {
        return 0;
    }

    // real msg size
    uint realSize = SMsgHeader::ParseSize(buff);
    if(realSize == 0)
    {
        return ERR_CORE_CLINET_READ_MSG_SIZE_ZERO;
    }
    if(realSize > MSG_MAX)
    {
        return ERR_CORE_CLINET_READ_MSG_SIZE_OVERFLOW;
    }
    if(realSize > (uint)copy)
    {
        return 0;
    }

    // remove from buffer
    if(evbuffer_drain(input, realSize))
    {
        return ERR_CORE_CLINET_READ_BUFF_DRAIN;
    }

    // monitor
    uint id = SMsgHeader::ParseId(buff);
    MONITOR(OnReadMsg(id, realSize));

    readSize = realSize;
    return 0;
}

uint CClient::Write(const char *buff, uint size)
{
    if(!buff)
    {
        return ERR_CORE_CLINET_WRITE_PARAM;
    }

    if(size > MSG_MAX)
    {
        return ERR_CORE_CLINET_WRITE_MSG_SIZE_OVERFLOW;
    }

    if(bufferevent_write(mBev, buff, size))
    {
        return ERR_CORE_CLINET_WRITE_FAILED;
    }

    // monitor
    uint id = SMsgHeader::ParseId(buff);
    MONITOR(OnWriteMsg(id, size));

    return 0;
}

uint CClient::Write(const CMsgBase *msg)
{
    if(!msg)
    {
        return ERR_CORE_CLINET_WRITE_PARAM;
    }

    char buff[MSG_MAX];
    CCoder coder(buff, sizeof(buff));
    msg->Encode(coder);
    if(coder.HasError())
    {
        return ERR_CORE_CLINET_WRITE_ENCODE;
    }

    return Write(coder.GetBuffer(), coder.GetCodedSize());
}

void CClient::Close()
{
    mState = kClientStateClosed;
    if(mBev)
    {
        bufferevent_free(mBev); // fd和buffer都会被释放
        mBev = NULL;
    }
}

// CClientPool
CClientPool::CClientPool()
{
    BatchCreate();
}

CClientPool::~CClientPool()
{
}

CClient* CClientPool::Get()
{
    if(!mList.size())
    {
        BatchCreate();
    }

    CClient *client = mList.front();
    mList.pop();
    MONITOR(OnClientPoolChanged(mList.size()));

    return client;
}

void CClientPool::Recycle(CClient *client)
{
    mList.push(client);
    MONITOR(OnClientPoolChanged(mList.size()));
}

const uint BATCH_COUNT = 20;
void CClientPool::BatchCreate()
{
    FOR(BATCH_COUNT, i)
    {
        CClient *client = new CClient();
        mList.push(client);
        MONITOR(OnClientCreated());
    }

    MONITOR(OnClientPoolChanged(mList.size()));
}

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
    m_host  = host;
    m_id    = 0;
    m_sid   = sid;
    m_bev   = bev;
    m_state = bev ? kClientStateConnected : kClientStateClosed;
    m_type  = bev ? kClientTypePassive    : kClientTypeActive;
}

uint CClient::Connect(event_base *base, FunConnected fun_connect)
{
    if(m_type != kClientTypeActive)
    {
        return ERR_CORE_CLINET_CONNECT_TYPE;
    }
    if(m_bev)
    {
        return ERR_CORE_CLINET_CONNECT_WITH_BUFFER;
    }

    // addr
    sockaddr_in addr;
    memset((void*)&addr, 0, sizeof(sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = CUtil::GetIpInt(m_host.ip);
    addr.sin_port        = htons(m_host.port);

    // connect
    m_state = kClientStateConnecting;
    m_bev   = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(m_bev, NULL, NULL, fun_connect, NULL);
    bufferevent_socket_connect(m_bev, (sockaddr*)&addr, sizeof(addr));

    MONITOR(OnOpenFd(GetFd()));

    return 0;
}

uint CClient::Read(char *buff, uint buff_size, uint &read_size)
{
    read_size = 0; // important

    if(!buff)
    {
        return ERR_CORE_CLINET_READ_PARAM;
    }
    if(!m_bev)
    {
        return ERR_CORE_CLINET_READ_NO_BUFF;
    }

    // copy
    evbuffer *input = bufferevent_get_input(m_bev);
    int copy = evbuffer_copyout(input, buff, buff_size);
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
    uint real_size = SMsgHeader::ParseSize(buff);
    if(real_size == 0)
    {
        return ERR_CORE_CLINET_READ_MSG_SIZE_ZERO;
    }
    if(real_size > MSG_MAX)
    {
        return ERR_CORE_CLINET_READ_MSG_SIZE_OVERFLOW;
    }
    if(real_size > (uint)copy)
    {
        return 0;
    }

    // remove from buffer
    if(evbuffer_drain(input, real_size))
    {
        return ERR_CORE_CLINET_READ_BUFF_DRAIN;
    }

    // monitor
    uint id = SMsgHeader::ParseId(buff);
    MONITOR(OnReadMsg(id, real_size));

    read_size = real_size;
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

    if(bufferevent_write(m_bev, buff, size))
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
    m_state = kClientStateClosed;
    if(m_bev)
    {
        bufferevent_free(m_bev); // fd和buffer都会被释放
        m_bev = NULL;
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
    if(!m_list.size())
    {
        BatchCreate();
    }

    CClient *client = m_list.front();
    m_list.pop();
    MONITOR(OnClientPoolChanged(m_list.size()));

    return client;
}

void CClientPool::Recycle(CClient *client)
{
    m_list.push(client);
    MONITOR(OnClientPoolChanged(m_list.size()));
}

const uint BATCH_COUNT = 20;
void CClientPool::BatchCreate()
{
    FOR(BATCH_COUNT, i)
    {
        CClient *client = new CClient();
        m_list.push(client);
        MONITOR(OnClientCreated());
    }

    MONITOR(OnClientPoolChanged(m_list.size()));
}

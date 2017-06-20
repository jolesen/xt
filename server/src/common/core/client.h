#ifndef _COMMON_CORE_CLIENT_H_
#define _COMMON_CORE_CLIENT_H_

#include <event2/bufferevent.h>
#include "event.h"
#include "util.h"
#include "msg.h"

// client type
const uint kClientTypePassive = 1;
const uint kClientTypeActive  = 2;

// client state
const uint kClientStateClosed     = 1;
const uint kClientStateConnecting = 2;
const uint kClientStateConnected  = 3;

// CClient
class CClientPool;
class CClient
{
public:
    typedef void (*FunConnected)(bufferevent *bev, short events, void *args);

    void SetPassive(const SHost &host, bufferevent *bev); // accepted
    void SetActive(const SHost &host, const std::string &sid); // connect to
    uint Connect(event_base *base, FunConnected fun_connected);
    uint Read(char *buff, uint buff_size, uint &read_size);
    uint Write(const char *buff, uint size);
    uint Write(const CMsgBase *msg);
    void Close();

    uint GetId() const { return m_id; }
    uint GetState() const { return m_state; }
    uint GetType() const { return m_type; }
    uint GetFd() const { return m_bev ? bufferevent_getfd(m_bev) : 0; }
    const SHost& GetHost() const { return m_host; }
    const std::string& GetServerId() const { return m_sid; }
    void SetId(uint value) { m_id = value; }
    void SetState(uint state) { m_state = state; }

private:
    friend class CClientPool;
    CClient();
    ~CClient();

private:
    void Init(const SHost &host, bufferevent *bev, const std::string &sid);

private:
    bufferevent* m_bev;
    std::string  m_sid;
    SHost        m_host;
    uint         m_id;
    uint         m_state;
    uint         m_type;
};
typedef std::unordered_map<uint, CClient*> FdClientMap; // key - fd
typedef std::unordered_map<uint, CClient*> IdClientMap; // key - id


// CClientPool
class CClientPool
{
public:
    CClient* Get();
    void Recycle(CClient *client);
    uint GetPoolSize() { return m_list.size(); }

private:
    friend CClientPool& CSingleton<CClientPool>::Instance();
    CClientPool();
    ~CClientPool();
    void BatchCreate();

private:
    std::queue<CClient*> m_list;
};
#define theClientPool CSingleton<CClientPool>::Instance()


// ------------- EVENT ------------- 
struct SEventClientAccepted : public SEventBase
{
    const CClient &client;
    SEventClientAccepted(const CClient &c) : client(c) { }
};
struct SEventClientClosed : public SEventBase
{
    const CClient &client;
    SEventClientClosed(const CClient &c) : client(c) { }
};

struct SEventLinkConnected : public SEventBase
{
    CClient &client;
    SEventLinkConnected(CClient &c) : client(c) { }
};
struct SEventLinkClosed : public SEventBase
{
    CClient &client;
    SEventLinkClosed(CClient &c) : client(c) { }
};

#endif

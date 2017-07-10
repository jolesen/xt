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
    void SetActive(const SHost &host, const string &sid); // connect to
    uint Connect(event_base *base, FunConnected funConnected);
    uint Read(char *buff, uint buffSize, uint &readSize);
    uint Write(const char *buff, uint size);
    uint Write(const CMsgBase *msg);
    void Close();

    uint GetId() const { return mId; }
    uint GetState() const { return mState; }
    uint GetType() const { return mType; }
    uint GetFd() const { return mBev ? bufferevent_getfd(mBev) : 0; }
    const SHost& GetHost() const { return mHost; }
    const string& GetServerId() const { return mSid; }
    void SetId(uint value) { mId = value; }
    void SetState(uint state) { mState = state; }

private:
    friend class CClientPool;
    CClient();
    ~CClient();

private:
    void Init(const SHost &host, bufferevent *bev, const string &sid);

private:
    bufferevent* mBev;
    string       mSid;
    SHost        mHost;
    uint         mId;
    uint         mState;
    uint         mType;
};
typedef std::unordered_map<uint, CClient*> FdClientMap; // key - fd
typedef std::unordered_map<uint, CClient*> IdClientMap; // key - id


// CClientPool
class CClientPool
{
public:
    CClient* Get();
    void Recycle(CClient *client);
    uint GetPoolSize() { return mList.size(); }

private:
    friend CClientPool& CSingleton<CClientPool>::Instance();
    CClientPool();
    ~CClientPool();
    void BatchCreate();

private:
    std::queue<CClient*> mList;
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

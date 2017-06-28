#ifndef _COMMON_CORE_SERVER_H_
#define _COMMON_CORE_SERVER_H_

#include <event2/listener.h>
#include "client.h"
#include "http.h"

// CServer
class CServer
{
public:
    uint Create();
    uint Start();
    void CloseClient(uint fd, uint reason);
    CClient* GetClientByFd(uint fd);
    CClient* GetClientById(uint id);

    event_base* GetEventBase() { return mBase; }
    uint GetListenFd() { return evconnlistener_get_fd(mListener); }
    uint GetClientCount() { return mFdClient.size(); }
    FdClientMap& GetFdClient() { return mFdClient; }
    uint GetStartTime() { return mStartTime; }
    uint GetFdErrorCode(short events);

private:
    friend CServer& CSingleton<CServer>::Instance();
    CServer();
    ~CServer();

    static void OnAccept(evconnlistener *listener, evutil_socket_t fd, sockaddr *addr, int socklen, void *args);
    static void OnListenError(evconnlistener *listener, void *args);
    static void OnFdError(bufferevent *bev, short events, void *args);
    static void OnRead(bufferevent *bev, void *args);

private:
    FdClientMap     mFdClient;
    IdClientMap     mIdClient;
    event_base*     mBase;
    evconnlistener* mListener;
    CHttpServer     mHttp;
    uint            mStartTime;
};
#define theServer CSingleton<CServer>::Instance()

#endif

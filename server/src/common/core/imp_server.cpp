#include <cstring>
#include "errorcode.h"
#include "sconfig.h"
#include "monitor.h"
#include "linker.h"
#include "server.h"
#include "time.h"
#include "log.h"

CServer::CServer()
{
    mListener = NULL;
}

CServer::~CServer()
{
}

uint CServer::Create()
{
    // addr
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(theServerConfig.host.port);

    // event base
    mBase = event_base_new();
    if(!mBase)
    {
        return ERR_CORE_SERVER_CREATE_NEW_BASE;
    }

    // listener
    uint opt = LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE;
    mListener = evconnlistener_new_bind(mBase, CServer::OnAccept, NULL, opt, theServerConfig.backlog, (sockaddr*)&addr, sizeof(addr));
    if(!mListener)
    {
        return ERR_CORE_SERVER_CREATE_NEW_LISTENER;
    }
    evconnlistener_set_error_cb(mListener, CServer::OnListenError);

    // http server
    uint ret = mHttp.Create(mBase);
    if(ret)
    {
        return ret;
    }

    LOG_INFO("<<< Server[%s, %s:%u] created, listen(fd=%u) >>>", theServerConfig.sid.c_str(), theServerConfig.host.ip.c_str(), theServerConfig.host.port, GetListenFd());
    return 0;
}

uint CServer::Start()
{
    // daemon
    if(!CUtil::StartupAsDaemon())
    {
        return ERR_CORE_SERVER_START_DAEMON;
    }

    mStartTime = CTime::Now();

    // monitor
    MONITOR(Start());

    // event loop
    event_base_dispatch(mBase);

    // free
    evconnlistener_free(mListener);
    mHttp.Free();
    event_base_free(mBase);

    return 0;
}

void CServer::OnAccept(evconnlistener *listener, evutil_socket_t fd, sockaddr *addr, int socklen, void *args)
{
    bufferevent *bev = NULL;
    uint error = 0;
    do
    {
        if(theServer.GetClientCount() >= theServerConfig.clientLimit)
        {
            error = ERR_CORE_SERVER_ACCEPT_CLIENT_MAX;
            break;
        }

        if(theServer.GetClientByFd(fd))
        {
            error = ERR_CORE_SERVER_ACCEPT_FD_HAD_CLINET;
            break;
        }

        event_base *base = evconnlistener_get_base(listener);
        if(!base)
        {
            error = ERR_CORE_SERVER_ACCEPT_NO_EVENT_BASE;
            break;
        }

        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        if(!bev)
        {
            error = ERR_CORE_SERVER_ACCEPT_CREATE_BUFFER;
            break;
        }
    }
    while(false);
    if(error)
    {
        LOG_ERROR("Accepted error, fd:%u, reason:%d", fd, error);
        theServer.CloseClient(fd, error);
        return;
    }

    // create client, don't care write event
    sockaddr_in *addrIn = (sockaddr_in*)addr;
    SHost host(CUtil::GetIpString(addrIn->sin_addr.s_addr), ntohs(addrIn->sin_port));
    bufferevent_enable(bev, EV_PERSIST | EV_READ | EV_ET);
    bufferevent_setcb(bev, CServer::OnRead, NULL, CServer::OnFdError, NULL);
    CClient *client = theClientPool.Get();
    if(!client)
    {
        LOG_ERROR("can not create client for accepted fd:%u, host:%s:%u", fd, host.ip.c_str(), host.port);
        return;
    }
    client->SetPassive(host, bev);
    FdClientMap &mpClient = theServer.GetFdClient();
    mpClient[fd] = client;

    // event
    EVENT(SEventClientAccepted, *client);

    MONITOR(OnOpenFd(fd));
}

void CServer::OnListenError(evconnlistener *listener, void *args)
{
    LOG_ERROR("Listen error");
}

void CServer::OnRead(bufferevent *bev, void *args)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    CClient *client = theServer.GetClientByFd(fd);
    if(!client)
    {
        theServer.CloseClient(fd, ERR_CORE_SERVER_ONREAD_NO_CLIENT);
        return;
    }

    char buff[MSG_MAX];
    uint readSize = 0;
    for(; ;)
    {
        uint ret = client->Read(buff, MSG_MAX, readSize);
        if(ret)
        {
            theServer.CloseClient(fd, ret);
            break;
        }

        if(!readSize)
        {
            break;
        }

        theServerConfig.funHandleMsg(*client, buff, readSize);
    }
}

void CServer::OnFdError(bufferevent *bev, short events, void *args)
{
    if(events & BEV_EVENT_CONNECTED)
    {
        LOG_ERROR("error to goto here");
        return;
    }

    uint fd     = bufferevent_getfd(bev);
    uint reason = theServer.GetFdErrorCode(events);
    theServer.CloseClient(fd, reason);
}

void CServer::CloseClient(uint fd, uint reason)
{
    CClient *client = GetClientByFd(fd);
    if(!client)
    {
        int ret = evutil_closesocket(fd);
        LOG_DEBUG("close fd=%u, reason=%u, ret:%d", fd, reason, ret);
        return;
    }

    uint realFd = client->GetFd();
    if(realFd != fd)
    {
        LOG_ERROR("close client error, realFd=%u != given_fd=%u", realFd, fd);
        return;
    }

    EVENT(SEventClientClosed, *client);
    mFdClient.erase(fd);
    mIdClient.erase(client->GetId());
    client->Close();
    theClientPool.Recycle(client);
    LOG_DEBUG("Close fd=%u, reason=%u", fd, reason);
}

CClient* CServer::GetClientByFd(uint fd)
{
    IF_FIND(mFdClient, fd, iter)
    {
        return iter->second;
    }

    return NULL;
}

CClient* CServer::GetClientById(uint id)
{
    IF_FIND(mIdClient, id, iter)
    {
        return iter->second;
    }

    return NULL;
}

uint CServer::GetFdErrorCode(short events)
{
    if(events & BEV_EVENT_EOF)
    {
        return ERR_CORE_SERVER_ONERROR_ACTIVE_CLOSED;
    }
    else if(events & BEV_EVENT_ERROR)
    {
        return ERR_CORE_SERVER_ONERROR_CLIENT_RESET;
    }
    else if(events & BEV_EVENT_READING)
    {
        return ERR_CORE_SERVER_ONERROR_CLIENT_READING;
    }
    else if(events & BEV_EVENT_WRITING)
    {
        return ERR_CORE_SERVER_ONERROR_CLIENT_WRITING;
    }
    else if(events & BEV_EVENT_TIMEOUT)
    {
        return ERR_CORE_SERVER_ONERROR_CLIENT_TIMEOUT;
    }

    return 0;
}

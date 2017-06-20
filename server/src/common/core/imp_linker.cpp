#include "errorcode.h"
#include "sconfig.h"
#include "monitor.h"
#include "server.h"
#include "linker.h"
#include "client.h"
#include "log.h"

CLinker::CLinker()
{
}

CLinker::~CLinker()
{
}

void CLinker::Link(const std::string &sid, const SHost &host)
{
    IF_NOTFIND(m_links, sid, iter)
    {
        CClient *client = theClientPool.Get();
        if(!client)
        {
            LOG_ERROR("can not create link for sid:%s, %s:%u", sid.c_str(), host.ip.c_str(), host.port);
            return;
        }

        client->SetActive(host, sid);
        m_links[sid] = client;

        MONITOR(AddLinkState(sid, kClientStateClosed, 0));
        LOG_INFO("create link to sid:%s, %s:%u", sid.c_str(), host.ip.c_str(), host.port);
    }
}

void _CloseLink(CClient &client, uint reason)
{
    const SHost &host = client.GetHost();
    LOG_INFO("link fd:%u close, host[%s %s:%u], reason:%u", client.GetFd(), client.GetServerId().c_str(), host.ip.c_str(), host.port, reason);

    EVENT(SEventLinkClosed, client);
    client.Close();

    MONITOR(AddLinkState(client.GetServerId(), kClientStateClosed, reason));
}

void CLinker::OnLinkRead(bufferevent *bev, void *args)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    CClient *client = theLinker.GetLinkByFd(fd);
    if(!client)
    {
        LOG_ERROR("on link read, can not find link by fd=%u", fd);
        return;
    }

    char buff[MSG_MAX];
    uint read_size = 0;
    for(; ;)
    {
        uint ret = client->Read(buff, MSG_MAX, read_size);
        if(ret)
        {
            _CloseLink(*client, ret);
            break;
        }

        if(!read_size)
        {
            break;
        }

        theServerConfig.msg_handler(*client, buff, read_size);
    }
}

void CLinker::OnLinkFdError(bufferevent *bev, short events, void *args)
{
    uint fd = bufferevent_getfd(bev);
    CClient *client = theLinker.GetLinkByFd(fd);
    if(!client)
    {
        LOG_ERROR("on link fd error, can not find link by fd=%u", fd);
        return;
    }

    uint reason = theServer.GetFdErrorCode(events);
    _CloseLink(*client, reason);
}

void CLinker::OnLinkConnected(bufferevent *bev, short events, void *args)
{
    uint fd = bufferevent_getfd(bev);
    CClient *client = theLinker.GetLinkByFd(fd);
    if(!client)
    {
        LOG_ERROR("can not find link by fd=%u", fd);
        return;
    }

    // connected
    if(events & BEV_EVENT_CONNECTED)
    {
        bufferevent_enable(bev, EV_PERSIST | EV_READ | EV_ET);
        bufferevent_setcb(bev, CLinker::OnLinkRead, NULL, CLinker::OnLinkFdError, args);
        client->SetState(kClientStateConnected);
        EVENT(SEventLinkConnected, *client);

        MONITOR(AddLinkState(client->GetServerId(), kClientStateConnected, 0));
    }
    else
    {
        _CloseLink(*client, ERR_CORE_CLINET_CONNECT_FAILED);
    }
}

void CLinker::CheckLink()
{
    FORMAP(m_links, iter)
    {
        CClient &client = *(iter->second);
        if(client.GetState() == kClientStateClosed)
        {
            uint ret = client.Connect(theServer.GetEventBase(), CLinker::OnLinkConnected);
            if(ret)
            {
                _CloseLink(client, ret);
            }
        }
    }
}

CClient* CLinker::GetLink(const std::string &sid)
{
    IF_FIND(m_links, sid, iter)
    {
        return iter->second;
    }

    return NULL;
}

CClient* CLinker::GetLinkByFd(uint fd)
{
    FORMAP(m_links, iter)
    {
        if(iter->second->GetFd() == fd)
        {
            return iter->second;
        }
    }

    return NULL;
}

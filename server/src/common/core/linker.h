#ifndef _COMMON_CORE_LINKER_H_
#define _COMMON_CORE_LINKER_H_

#include <event2/event.h>
#include "client.h"

// CLinker
class CLinker
{
public:
    CClient* GetLink(const std::string &sid);
    void CheckLink();
    void Link(const std::string &sid, const SHost &host);
    uint GetLinkCount() { return mLinks.size(); }

public:
    static void OnLinkRead(bufferevent *bev, void *args);
    static void OnLinkConnected(bufferevent *bev, short events, void *args);
    static void OnLinkFdError(bufferevent *bev, short events, void *args);

private:
    friend CLinker& CSingleton<CLinker>::Instance();
    CLinker();
    ~CLinker();
    CClient* GetLinkByFd(uint fd);

private:
    std::map<std::string, CClient*> mLinks;
};
#define theLinker CSingleton<CLinker>::Instance()

#endif

#include "event.h"
#include "log.h"

CEvent::CEvent()
{
}

CEvent::~CEvent()
{
}

bool CEvent::Register(const string &strName, const string &strFile, SEventInfo::FunEvent handler)
{
    SEventInfo info(strFile, handler);
    mHandlers[strName].push_back(info);
    return true;
}

void CEvent::Execute(const string &strName, SEventBase *event)
{
    IF_NFIND(mHandlers, strName, iterMap)
    {
        return;
    }

    std::vector<SEventInfo> &list = iterMap->second;
    for(std::vector<SEventInfo>::iterator iter = list.begin(); iter != list.end(); )
    {
        event->isDelete = false;
        (iter->handler)(event);
        if(event->isDelete)
        {
            LOG_INFO("Remove event(%s)'s one callback [%s]", iterMap->first.c_str(), iter->path.c_str());
            iter = list.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

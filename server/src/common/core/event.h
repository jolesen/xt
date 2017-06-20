#ifndef _COMMON_CORE_EVENT_H_
#define _COMMON_CORE_EVENT_H_

#include "core.h"

// SEventBase
struct SEventBase
{
    bool is_delete;
};

// CEvent
class CEvent
{
private:
    struct SEventInfo
    {
        typedef void (*FunEvent)(SEventBase *event);

        std::string path;
        FunEvent    handler;
        SEventInfo(const std::string &p, FunEvent h) : path(p), handler(h) { }
    };

public:
    bool Register(const std::string &strName, const std::string &strFile, SEventInfo::FunEvent handler);
    void Execute (const std::string &strName, SEventBase *event);

private:
    friend CEvent& CSingleton<CEvent>::Instance();
    CEvent();
    ~CEvent();

private:
    std::map<std::string, std::vector<SEventInfo> > m_handlers;
};
#define theEvent CSingleton<CEvent>::Instance()

// EVENT_FUNC (E - event)
#define EVENT_FUNC(E)                                                   \
    void On##E(E& ev);                                                  \
    void CastEvent##E(SEventBase *ev) { On##E(*((E*)ev)); }             \
    bool gRegEvent##E = theEvent.Register(#E, __FILE__, CastEvent##E);  \
    void On##E(E& ev)

// DISPATCH EVENT(E - event, ... - constructor arguments)
#define EVENT(E, ...)  { E evt = E(__VA_ARGS__); theEvent.Execute(#E, (SEventBase*)(&evt)); }

#endif

#ifndef _COMMON_CORE_EVENT_H_
#define _COMMON_CORE_EVENT_H_

#include "core.h"

// SEventBase
struct SEventBase
{
    bool isDelete;
};

// CEvent
class CEvent
{
private:
    struct SEventInfo
    {
        typedef void (*FunEvent)(SEventBase *event);

        string   path;
        FunEvent handler;
        SEventInfo(const string &p, FunEvent h) : path(p), handler(h) { }
    };

public:
    bool Register(const string &strName, const string &strFile, SEventInfo::FunEvent handler);
    void Execute (const string &strName, SEventBase *event);

private:
    friend CEvent& CSingleton<CEvent>::Instance();
    CEvent();
    ~CEvent();

private:
    std::map<string, std::vector<SEventInfo> > mHandlers;
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

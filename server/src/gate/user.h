#ifndef _GATE_GATE_H_
#define _GATE_GATE_H_

#include "common/core/core.h"

typedef std::map<uint, uint> LoginingMap;

// CUser
class CUser
{
public:
    CUser() { }
    ~CUser(){ }

public:
    LoginingMap& GetLoginings() { return m_loginings; }

private:
    LoginingMap m_loginings;
};
#define theUser CSingleton<CUser>::Instance()

#endif

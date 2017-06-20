#ifndef _LOGIC_LOGIN_H_
#define _LOGIC_LOGIN_H_

#include "common/core/event.h"
#include "common/msg/user.h"

// SEventLogin
struct SEventLogin : public SEventBase
{
    SUser& user;
    SEventLogin(SUser &u) : user(u) { }
};

// SEventLogout
struct SEventLogout : public SEventBase
{
    SUser& user;
    SEventLogout(SUser &u) : user(u) { }
};

#endif

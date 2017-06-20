#ifndef _AUTH_USER_H_
#define _AUTH_USER_H_

#include "common/core/core.h"

// CUsers
class CUsers
{
public:
    CUsers() { }
    ~CUsers(){ }

public:
    void Add(uint uid, uint session);
    bool Auth(uint uid, uint session);

private:
    typedef std::map<uint, S2UInt> AuthUserMap;

    AuthUserMap m_users;
};
#define theUsers CSingleton<CUsers>::Instance()

#endif

#include "user.h"

void CUsers::Add(uint uid, uint session)
{
    S2UInt user;
    user.id    = uid;
    user.value = session;

    m_users[uid] = user;
}

bool CUsers::Auth(uint uid, uint session)
{
    IF_FIND(m_users, uid, iter)
    {
        if(iter->second.value == session)
        {
            return true;
        }
    }

    //return false;
    return true; // TEST
}

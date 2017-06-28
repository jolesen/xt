#include "user.h"

void CUsers::Add(uint uid, uint session)
{
    S2UInt user;
    user.id    = uid;
    user.value = session;

    mUsers[uid] = user;
}

bool CUsers::Auth(uint uid, uint session)
{
    IF_FIND(mUsers, uid, iter)
    {
        if(iter->second.value == session)
        {
            return true;
        }
    }

    //return false;
    return true; // TEST
}

#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/misc.h"
#include "module.h"
#include "user.h"

namespace user
{

void Add(SUser &user)
{
    theData.users[user.base.id] = user;

    SUser &u = theData.users[user.base.id];
    module::InitMd5(u);
}

bool Auth(uint uid, uint session)
{
    IF_FIND(theData.users, uid, iter)
    {
        return (iter->second.base.session == session);
    }

    return false;
}

SUser* Find(uint uid)
{
    IF_FIND(theData.users, uid, iter)
    {
        return &(iter->second);
    }

    return NULL;
}

SUser* FindBySession(uint uid, uint session)
{
    SUser *user = Find(uid);
    if(!user)
    {
        LOG_ERROR("Can't find user=%u", uid);
        return NULL;
    }

    if(user->base.session != session)
    {
        LOG_ERROR("MSG ERROR, uid=%u, session=%u(!=%u)", uid, session, user->base.session);
        return NULL;
    }

    return user;
}

UserMap& GetUsers()
{
    return theData.users;
}

void Logout(uint uid, uint reason)
{
    QSysUserLogout req;
    req.uid = uid;
    req.reason = reason;

    //SEND_GATE(req);
}

void OnLogouted(uint uid, uint reason)
{
    IF_NFINDx(theData.users, uid)
    {
        LOG_ERROR("no user, uid=%u", uid);
        return;
    }

    theData.users.erase(uid);
    LOG_DEBUG("user=%u, reason=%u, logouted in logic", uid, reason);
}



}

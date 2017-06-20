#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/data/user.h"
#include "common/misc.h"
#include "login.h"
#include "user.h"

MSG_SYSTEM(QUserLogin)
{
    if(user::Auth(msg.uid, msg.session))
    {
        RUserLogin rsp;
        rsp.flag    = kErrorLoginedOnLogic;
        rsp.uid     = msg.uid;
        rsp.session = msg.session;
        //SEND_GATE(rsp);

        LOG_ERROR("already logined on logic, uid=%u", msg.uid);
    }
    else
    {
        QSysAuthUser req;
        req.uid     = msg.uid;
        req.session = msg.session;
        //SEND_AUTH(req);
    }
}

MSG_SYSTEM(RSysAuthUser)
{
    if(msg.authed)
    {
        QSysLoadUser req;
        req.uid     = msg.uid;
        req.session = msg.session;
        //SEND_DATA(req);
    }
    else
    {
        RUserLogin rsp;
        rsp.flag    = kErrorLoginAuthedFailed;
        rsp.uid     = msg.uid;
        rsp.session = msg.session;
        //SEND_GATE(rsp);

        LOG_ERROR("auth failed, uid=%u,session=%u", msg.uid, msg.session);
    }
}

MSG_SYSTEM(RSysLoadUser)
{
    if(msg.flag == 0)
    {
        SUser user = msg.user;
        user.base.id      = msg.uid;
        user.base.session = msg.session;

        user::Add(user);
    }

    RUserLogin rsp;
    rsp.flag    = msg.flag;
    rsp.uid     = msg.uid;
    rsp.session = msg.session;
    //SEND_GATE(rsp);
}

MSG_SYSTEM(RSysUserLogout)
{
    user::OnLogouted(msg.uid, msg.reason);
}

#include "cacher.h"

/*
MSG_SYSTEM(QSysLoadUser)
{
    const SUser *user = theCacher.Find(msg.uid);
    if(user)
    {
        RSysLoadUser rsp;
        rsp.uid     = msg.uid;
        rsp.session = msg.session;
        rsp.user    = *user;

        SEND_LOGIC(rsp);
    }
    else
    {
        theCacher.Load(msg.uid, msg.session);
    }
}

MSG_SYSTEM(QSysSaveUser)
{
    theCacher.Save(msg.uid, msg.modules);
}
*/

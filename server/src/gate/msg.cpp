#include "common/core/errorcode.h"
#include "common/core/server.h"
#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/data/user.h"
#include "common/misc.h"
#include "user.h"

MSG_FUNC(QUserLogin)
{
    LoginingMap &map = theUser.GetLoginings();
    IF_NFINDx(map, msg.uid)
    {
        map[msg.uid] = msg.header.fd;

        QUserLogin req = msg;
        //SEND_LOGIC(req);

        LOG_DEBUG("login requst(uid=%u, session=%u) sent to logicsvr", msg.uid, msg.session);
    }
    else
    {
        RUserLogin rsp;
        rsp.flag    = kErrorLoginingOnGate;
        rsp.uid     = msg.uid;
        rsp.session = msg.session;
        CMisc::SendUser(&rsp, msg.uid);

        LOG_ERROR("already logining now, guid=%u", msg.uid);
    }
}

MSG_SYSTEM(RUserLogin)
{
    LoginingMap &map = theUser.GetLoginings();
    IF_NFIND(map, msg.uid, iter)
    {
        LOG_ERROR("no fd by uid =%u", msg.uid);
        return;
    }

    CClient *client = theServer.GetClientByFd(iter->second);
    if(!client)
    {
        LOG_ERROR("no logining client, and close fd, fd=%u, guid=%u", iter->second, msg.uid);
        theServer.CloseClient(iter->second, 11111);
        return;
    }

    if(msg.flag != 0)
    {
        LOG_ERROR("login failed and close fd, fd=%u, guid=%u, session=%u", iter->second, msg.uid, msg.session);
        theServer.CloseClient(iter->second, 1111111111);
        return;
    }

    // success
    client->SetId(msg.uid);
    map.erase(msg.uid);

    RUserLogin rsp = msg;
    CMisc::SendUser(&rsp, rsp.uid);

    LOG_DEBUG("login success, fd=%u, guid=%u, session=%u", iter->second, msg.uid, msg.session);
}

MSG_SYSTEM(QSysUserLogout)
{
    CClient *client = theServer.GetClientById(msg.uid);
    if(client)
    {
        theServer.CloseClient(client->GetFd(), msg.reason);
    }
    else
    {
        LOG_ERROR("no client by user=%u", msg.uid);
    }
}

MSG_SYSTEM(QSysBroadcast)
{
    CClient *client = NULL;
    uint ret = 0;
    if(msg.guids.size() == 0) // BROADCAST_ALL
    {
        FdClientMap &map = theServer.GetFdClient();
        FORMAP(map, iter)
        {
            client = iter->second;
            if(client && (client->GetType() == kClientTypePassive))
            {
                ret = client->Write(msg.msg.bytes, msg.msg.size);
                if(ret != 0)
                {
                    LOG_ERROR("broadcast to user failed, ret=%u", ret);
                }
            }
        }
    }
    else // BROADCAST_LIST
    {
        FORLIST(msg.guids, i)
        {
            client = theServer.GetClientById(msg.guids[i]);
            if(client && (client->GetType() == kClientTypePassive))
            {
                ret = client->Write(msg.msg.bytes, msg.msg.size);
                if(ret != 0)
                {
                    LOG_ERROR("broadcast to user failed, ret=%u", ret);
                }
            }
        }
    }
}

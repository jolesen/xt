#include "core/sconfig.h"
#include "core/server.h"
#include "core/linker.h"
#include "core/signal.h"
#include "core/timer.h"
#include "core/util.h"
#include "core/log.h"
#include "msg/system.h"
#include "msg/idname.h"
#include "config.h"
#include "misc.h"

// ------------------------- CMisc ------------------------
void CMisc::BroadcastUsers(CMsgBase *msg, uint type, const UIntList *users)
{
    if(!msg)
    {
        return;
    }

    QSysBroadcast req;
    CCoder coder(req.msg.bytes, sizeof(req.msg.bytes));
    msg->Encode(coder);
    if(coder.HasError())
    {
        LOG_ERROR("BroadcastMsgEncodeError");
        return;
    }
    req.msg.size = coder.GetCodedSize();

    // broadcast to all
    if(!users)
    {
        //SEND_GATE(req);
        return;
    }

    // broadcast to given users
    const UIntList &list = *users;
    FORLIST(list, i)
    {
        req.guids.push_back(list[i]);

        if(req.guids.size() >= theConfig.broadcastMax)
        {
            //SEND_GATE(req);
            req.guids.clear();
        }
    }
    if(req.guids.size() > 0) // last
    {
        //SEND_GATE(req);
    }
}

void CMisc::SendUser(CMsgBase *msg, uint uid)
{
}

bool CMisc::CheckSystemMessage(const CMsgBase *msg)
{
    //CClient *link_check = theServer.GetLinkout(msg->header.src);
    CClient *link_check = NULL; // TODO
    if(!link_check)
    {
        //LOG_ERROR("Can't find link, type=%u", msg->header.src);
        return false;
    }
    //if(msg->header.guid != link_check->GetId())
    {
        //LOG_ERROR("MSG ERROR, header.guid=%u(!=link.guid=%u)", msg->header.guid, link_check->GetId());
        return false;
    }

    return true;
}

uint _GetMsgId(const string &msg_name)
{
    return theMsgIdName.GetMsgId(msg_name);
}

string _GetMsgName(uint msg_id)
{
    return theMsgIdName.GetMsgName(msg_id);
}

uint CMisc::Main(uint argc, const char **argv, FunServerLoadData fun_load, FunServerInit fun_init)
{
    printf(">>>>> Starting... ");
    fflush(stdout);

    theSignal.Init();
    do
    {
        // 启动参数
        string load = theConfig.Parse(argc, argv);
        if(load.size())
        {
            printf("ParseConfig error, %s\n", load.c_str());
            break;
        }

        // 设置日志名称
        theLog.SetFileName(theConfig.sid + ".log");
        theConfig.Show();

        // 加载静态数据
        if(fun_load)
        {
            if(!fun_load(theConfig.resource + "/"))
            {
                printf("%s, LoadConfig error.\n", theConfig.sid.c_str());
                break;
            }
        }

        // 创建服务器
        theServerConfig.type          = theConfig.type;
        theServerConfig.sid           = theConfig.sid;
        theServerConfig.host          = theConfig.host;
        theServerConfig.httpHost      = theConfig.httpHost;
        theServerConfig.mongoHost     = theConfig.mongoHost;
        theServerConfig.redisHost     = theConfig.redisHost;
        theServerConfig.backlog       = theConfig.backlog;
        theServerConfig.clientLimit   = theConfig.clientLimit;
        theServerConfig.monitorTick   = theConfig.monitorTick;
        theServerConfig.funHandleMsg  = (theConfig.type == kServiceGate) ? CMisc::HandleGateMsg : CMisc::HandleDefaultMsg;
        theServerConfig.funGetMsgId   = _GetMsgId;
        theServerConfig.funGetMsgName = _GetMsgName;
        uint ret = theServer.Create();
        if(ret)
        {
            printf("%s, CreateServer error, ret:%u\n", theConfig.sid.c_str(), ret);
            break;
        }

        // 初始化定时器
        theTimer.Init(theServer.GetEventBase());

        // 执行初始化逻辑
        if(fun_init && !fun_init())
        {
            printf("%s, InitServer error.\n", theConfig.sid.c_str());
            break;
        }

        // 默认连接center
        if(theConfig.type != kServiceCenter)
        {
            FORMAP(theConfig.centers, iter)
            {
                theLinker.Link(iter->first, iter->second);
            }
        }

        // 提示服务器启动成功并开始监听消息
        printf("DONE -> [%s, %s:%u]\n", theConfig.sid.c_str(), theConfig.host.ip.c_str(), theConfig.host.port);
        ret = theServer.Start();
        if(ret)
        {
            printf("Start error, ret:%u\n", ret);
        }
    }
    while(false);

    return 0;
}

void CMisc::OnConnected(CClient &client)
{
    const SHost &host = client.GetHost();
    LOG_INFO("connected to host[%s, %s:%u], fd=%u", client.GetServerId().c_str(), host.ip.c_str(), host.port, client.GetFd());

    // TODO 连接的是center才发送
    QSysRegistService msg;
    msg.service.sid  = theConfig.sid;
    msg.service.ip   = theConfig.host.ip;
    msg.service.port = theConfig.host.port;
    client.Write(&msg);
}

EVENT_FUNC(SEventClientAccepted)
{
    const SHost &host = ev.client.GetHost();
    LOG_DEBUG("accepted ok, ip=%s, port=%u, fd=%u", host.ip.c_str(), host.port, ev.client.GetFd());
}

EVENT_FUNC(SEventClientClosed)
{
    //LOG_DEBUG("client closed");
}

// --------------------- 消息处理 -------------------
bool _HandleMessage(CClient &client, char* buff, uint size, bool isShow)
{
    uint msgId = SMsgHeader::ParseId(buff);
    CMsgBase *msg = theMsgs.Create(msgId);
    if(!msg)
    {
        if(isShow)
        {
            LOG_ERROR("can not create msg [%u, %s], fd=%u, client:%u, size:%u", msgId, theMsgIdName.GetMsgName(msgId).c_str(), client.GetFd(), client.GetId(), size);
        }
        return false;
    }

    // decode
    CCoder coder(buff, size);
    msg->Decode(coder);
    if(coder.HasError())
    {
        LOG_ERROR("decode msg failed, msg=%u, fd=%u, client:%u", msgId, client.GetFd(), client.GetId());
        return true;
    }

    // execute
    LOG_DEBUG(">>> MSG[fd=%u, client:%u, size:%u]: %s", client.GetFd(), client.GetId(), size, theMsgIdName.GetMsgName(msgId).c_str());
    msg->header.fd = client.GetFd();
    theMsgs.Execute(msg);

    return true;
}

void CMisc::HandleDefaultMsg(CClient &client, char* buff, uint size)
{
    _HandleMessage(client, buff, size, true);
}

void CMisc::HandleGateMsg(CClient &client, char* buff, uint size)
{
    if(_HandleMessage(client, buff, size, false))
    {
        return;
    }

    // 进行消息转发
}

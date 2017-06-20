#ifndef _COMMON_MISC_H_
#define _COMMON_MISC_H_

#include "core/client.h"
#include "core/msg.h"

// -------------------------------------------
typedef bool (*FunServerInit)();
typedef bool (*FunServerLoadData)(const std::string &path);


// CMisc
class CMisc
{
public:
    static uint Main(uint argc, const char **argv, FunServerLoadData fun_load, FunServerInit fun_init);
    static void BroadcastUsers(CMsgBase *msg, uint type, const UIntList *users);
    static void SendUser(CMsgBase *msg, uint uid);
    static bool CheckSystemMessage(const CMsgBase *msg);
    static void OnConnected(CClient &client);
    static void HandleDefaultMsg(CClient &client, char* buff, uint size);
    static void HandleGateMsg(CClient &client, char* buff, uint size);
};

// -------------- MSG --------------
// MSG_SYSTEM
#define MSG_SYSTEM(T)\
    T *gMsg##T = new T;\
    void OnMsg##T(const T& msg);\
    void CastMsg##T(const CMsgBase* msg) { if(CMisc::CheckSystemMessage(msg)){ OnMsg##T(*((const T*)msg)); } }\
    bool gRegMsg##T = theMsgs.Register((CMsgBase*)(gMsg##T), CastMsg##T, #T);\
    void OnMsg##T(const T& msg)

// SEND MSG
#define SEND_USER(USER, MSG)       CMisc::SendUser (&(MSG), (USER).base.id)
#define SEND_USER_ERROR(USER, ERR) { RUserErrorCode msg; msg.code = (ERR); CMisc::SendUser(&msg, (USER).base.id); }
#define BROADCAST_ALL(MSG)         CMisc::BroadcastUsers(&(MSG), 0, NULL)
#define BROADCAST_LIST(MSG, LIST)  CMisc::BroadcastUsers(&(MSG), 0, &(LIST))

// Respond message for source message
#define RSP(FROM_MSG, SEND_MSG)\
    {\
        CClient *client = theServer.GetClientByFd(FROM_MSG.header.fd);\
        if(!client)\
        {\
            LOG_ERROR("can't find client by fd:%u", FROM_MSG.header.fd);\
            return;\
        }\
        uint ret = client->Write(&SEND_MSG);\
        if(ret)\
        {\
            LOG_ERROR("sent to fd:%u, ret:%u", FROM_MSG.header.fd, ret);\
        }\
    }


#endif

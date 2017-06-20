#ifndef _LOGIC_USER_H_
#define _LOGIC_USER_H_

#include "common/msg/user.h"
#include "common/misc.h"

// user
namespace user
{
    // data
    typedef std::map<uint, SUser> UserMap;
    struct SData
    {
        UserMap users;
    };
    #define theData CSingleton<SData>::Instance()

    // public
    void Add(SUser &user);
    bool Auth(uint uid, uint session);
    void Logout(uint uid, uint reason);
    void OnLogouted(uint uid, uint reason);
    SUser* Find(uint uid);
    UserMap& GetUsers();

    SUser* FindBySession(uint uid, uint session);
}

//SUser *user = user::FindBySession(msg->header.guid, msg->header.session);   

// MSG_USER
#define MSG_USER(T)                                                                 \
    T *gMsg##T = new T;                                                             \
    void OnMsg##T(SUser &user, const T& msg);                                       \
    void CastMsg##T(const CMsgBase* msg)                                            \
    {                                                                               \
        SUser *user = user::FindBySession(0, 0);                                    \
        if(user)                                                                    \
        {                                                                           \
            OnMsg##T(*user, *((const T*)msg));                                      \
        }                                                                           \
    }                                                                               \
    bool gRegMsg##T = theMsgs.Register((CMsgBase*)(gMsg##T), CastMsg##T, #T);       \
    void OnMsg##T(SUser &user, const T& msg)

#endif

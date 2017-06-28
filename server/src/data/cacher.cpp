#include "common/core/time.h"
#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/misc.h"
#include "cacher.h"

CCacher* CCacher::mInstance = NULL;

bool CCacher::Init()
{
    mInstance = this;

    if(!mLoader.Init())
    {
        return false;
    }

    if(!mSaver.Init())
    {
        return false;
    }

    return true;
}

void CCacher::Start()
{
    mLoader.Start();
    mSaver.Start();
}

SUser* CCacher::Find(uint uid)
{
    IF_FIND(mUsers, uid, iter)
    {
        return iter->second;
    }

    return NULL;
}

void CCacher::Load(uint uid, uint session)
{
    SUser *user = new SUser();
    user->base.id      = uid;
    user->base.session = session;

    mLoader.Load(user);
}

void CCacher::Save(uint uid, const std::map<uint, SBytes> &modules)
{
    FORMAP(modules, iter)
    {
        mSaver.DecodeModule(uid, iter->first, iter->second);
    }
}

void CCacher::CheckLoaded()
{
    std::vector<SUser*> list;
    mLoader.GetLoadedList(list);

    if(list.size() == 0)
    {
        return;
    }

    FORLIST(list, i)
    {
        SUser *user = list[i];

        uint flag = user->base.time;
        if(flag == 0)
        {
            user->base.time = CTime::Now();
            mUsers[user->base.id] = user;
        }
        else
        {
            LOG_ERROR("user(uid=%u, session=%u) load data failed, flag=%u", user->base.id, user->base.session, flag);
            continue;
        }

        // RSP
        RSysLoadUser rsp;
        rsp.flag    = flag;
        rsp.uid     = user->base.id;
        rsp.session = user->base.session;
        rsp.user    = *user;

        //SEND_LOGIC(rsp);

        LOG_INFO("user(uid=%u, session=%u) load data success, and sent to logic", user->base.id, user->base.session);
    }
}

void CCacher::Remove(uint uid)
{
    IF_FIND(mUsers, uid, iter)
    {
        SUser *user = iter->second;
        mUsers.erase(iter);

        delete user;
    }
}

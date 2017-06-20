#include "common/core/time.h"
#include "common/core/log.h"
#include "common/msg/system.h"
#include "common/misc.h"
#include "cacher.h"

CCacher* CCacher::m_instance = NULL;

bool CCacher::Init()
{
    m_instance = this;

    if(!m_loader.Init())
    {
        return false;
    }

    if(!m_saver.Init())
    {
        return false;
    }

    return true;
}

void CCacher::Start()
{
    m_loader.Start();
    m_saver.Start();
}

SUser* CCacher::Find(uint uid)
{
    IF_FIND(m_users, uid, iter)
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

    m_loader.Load(user);
}

void CCacher::Save(uint uid, const std::map<uint, SBytes> &modules)
{
    FORMAP(modules, iter)
    {
        m_saver.DecodeModule(uid, iter->first, iter->second);
    }
}

void CCacher::CheckLoaded()
{
    std::vector<SUser*> list;
    m_loader.GetLoadedList(list);

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
            m_users[user->base.id] = user;
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
    IF_FIND(m_users, uid, iter)
    {
        SUser *user = iter->second;
        m_users.erase(iter);

        delete user;
    }
}

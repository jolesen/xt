#include "common/data/user.h"
#include "common/core/util.h"
#include "common/core/log.h"
#include "cacher.h"
#include "dbio.h"

// CLoader
void CLoader::Load(SUser *user)
{
    if(user)
    {
        m_mutex_loading.Lock();
        {
            m_loadings[user->base.id] = user;
        }
        m_mutex_loading.Unlock();
    }
}

void CLoader::GetLoadedList(std::vector<SUser*> &list)
{
    m_mutex_loaded.Lock();
    {
        if(m_loadeds.size() > 0)
        {
            FORLIST(m_loadeds, i)
            {
                list.push_back(m_loadeds[i]);
            }
            m_loadeds.clear();
        }
    }
    m_mutex_loaded.Unlock();
}

void CLoader::Run()
{
    for(; ;)
    {
        SUser *user = NULL;

        m_mutex_loading.Lock();
        {
            LoadingMap::iterator iter_first = m_loadings.begin();
            if(iter_first != m_loadings.end())
            {
                user = iter_first->second;
                m_loadings.erase(iter_first);
            }
        }
        m_mutex_loading.Unlock();

        if(user)
        {
            user->base.time = 0;

            FORMAP(theModule.loaders, iter)
            {
                if(!(iter->second)(*user, m_mysql))
                {
                    user->base.time = kErrorLoginDataLoadFailed; // temp used as marker
                    break;
                }
            }

            m_mutex_loaded.Lock();
            {
                m_loadeds.push_back(user);
            }
            m_mutex_loaded.Unlock();
        }

        CUtil::SleepMillisecond(20);
    }
}

bool CLoader::Init()
{
    //return CDBIniter::InitMysql(m_mysql);
    return false;
}

// CSaver
void CSaver::Run()
{
    for(; ;)
    {
        DirtyUserMap dirties;
        m_mutex.Lock();
        {
            dirties = m_dirties;
            m_dirties.clear();
        }
        m_mutex.Unlock();

        FORMAP(dirties, iter)
        {
            SUser *user = theCacher.Find(iter->first);
            if(user)
            {
                LOG_DEBUG("---- SAVE USER UID=%u ----", user->base.id);

                FORMAP(user->inner.dirty_list, iter_module)
                {
                    if(iter_module->second)
                    {
                        Save(*user, iter_module->first);
                        iter_module->second = false;
                    }
                }
            }
        }

        CUtil::SleepMillisecond(10);
    }
}

void CSaver::DecodeModule(uint uid, uint module, const SBytes &data)
{
    SUser *user = theCacher.Find(uid);
    if(!user)
    {
        LOG_ERROR("no user in cache uid=%u", uid);
        return;
    }

    IF_NFIND(theModule.decoders, module, iter)
    {
        LOG_ERROR("no decoder for module=%u", module);
        return;
    }

    char* buff = (char*)(data.bytes);
    CCoder coder(buff, data.size);

    iter->second(*user, coder);
    user->inner.dirty_list[module] = true;

    // set dirty list
    m_mutex.Lock();
    {
        m_dirties[uid] = true;
    }
    m_mutex.Unlock();
}

void CSaver::Save(const SUser &user, uint module)
{
    IF_NFIND(theModule.savers, module, iter)
    {
        LOG_ERROR("no saver for module=%u", module);
        return;
    }

    StringList sqls;
    (iter->second)(user, sqls);

    // begin
    m_mysql.Execute("START TRANSACTION;");
    if(m_mysql.GetErrorCode() != 0)
    {
        LOG_ERROR("SQL start transaction occuried errors, error=[%u], des=[%s]", m_mysql.GetErrorCode(), m_mysql.GetErrorMsg());
        return;
    }

    // execute
    FORLIST(sqls, i)
    {
        const char* str_sql = sqls[i].c_str();
        m_mysql.Execute(str_sql);
        if(m_mysql.GetErrorCode() != 0)
        {
            LOG_ERROR("SQL execute occuried errors, sql=[%s], error=[%u], des=[%s]", str_sql, m_mysql.GetErrorCode(), m_mysql.GetErrorMsg());
            m_mysql.Execute("ROOLBACK;");
            return;
        }

        LOG_DEBUG("SQL = [%s]", str_sql);
    }

    // commit
    m_mysql.Execute("COMMIT;");
    if(m_mysql.GetErrorCode() != 0)
    {
        LOG_ERROR("SQL commit occuried errors, error=[%u], des=[%s]", m_mysql.GetErrorCode(), m_mysql.GetErrorMsg());
        return;
    }
}

bool CSaver::Init()
{
    /*
    const SDbCfg *cfg = theDataBaseCfg.GetDbCfg();
    if(!cfg)
    {
        LOG_ERROR("cannot get cfg of database");
        return false;
    }

    if(!mysql.TestConnect())
    {
        if(!mysql.Connect(*cfg))
        {
            LOG_ERROR("cannot connect to mysql [%s:%u], mysql=%s, usr=%s, pwd=%s",
                cfg->host.c_str(), cfg->port, cfg->mysql.c_str(), cfg->usr.c_str(), cfg->pwd.c_str());
            return false;
        }
        else
        {
            LOG_INFO("--- MYSQL CONNECTED ---");
        }
    }
    */

    return true;
}

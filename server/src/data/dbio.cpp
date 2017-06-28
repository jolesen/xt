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
        mMutexLoading.Lock();
        {
            mLoadings[user->base.id] = user;
        }
        mMutexLoading.Unlock();
    }
}

void CLoader::GetLoadedList(std::vector<SUser*> &list)
{
    mMutexLoaded.Lock();
    {
        if(mLoadeds.size() > 0)
        {
            FORLIST(mLoadeds, i)
            {
                list.push_back(mLoadeds[i]);
            }
            mLoadeds.clear();
        }
    }
    mMutexLoaded.Unlock();
}

void CLoader::Run()
{
    for(; ;)
    {
        SUser *user = NULL;

        mMutexLoading.Lock();
        {
            LoadingMap::iterator iterFirst = mLoadings.begin();
            if(iterFirst != mLoadings.end())
            {
                user = iterFirst->second;
                mLoadings.erase(iterFirst);
            }
        }
        mMutexLoading.Unlock();

        if(user)
        {
            user->base.time = 0;

            FORMAP(theModule.loaders, iter)
            {
                if(!(iter->second)(*user, mMysql))
                {
                    user->base.time = kErrorLoginDataLoadFailed; // temp used as marker
                    break;
                }
            }

            mMutexLoaded.Lock();
            {
                mLoadeds.push_back(user);
            }
            mMutexLoaded.Unlock();
        }

        CUtil::SleepMillisecond(20);
    }
}

bool CLoader::Init()
{
    //return CDBIniter::InitMysql(mMysql);
    return false;
}

// CSaver
void CSaver::Run()
{
    for(; ;)
    {
        DirtyUserMap dirties;
        mMutex.Lock();
        {
            dirties = mDirties;
            mDirties.clear();
        }
        mMutex.Unlock();

        FORMAP(dirties, iter)
        {
            SUser *user = theCacher.Find(iter->first);
            if(user)
            {
                LOG_DEBUG("---- SAVE USER UID=%u ----", user->base.id);

                FORMAP(user->inner.dirtyList, iterModule)
                {
                    if(iterModule->second)
                    {
                        Save(*user, iterModule->first);
                        iterModule->second = false;
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
    user->inner.dirtyList[module] = true;

    // set dirty list
    mMutex.Lock();
    {
        mDirties[uid] = true;
    }
    mMutex.Unlock();
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
    mMysql.Execute("START TRANSACTION;");
    if(mMysql.GetErrorCode() != 0)
    {
        LOG_ERROR("SQL start transaction occuried errors, error=[%u], des=[%s]", mMysql.GetErrorCode(), mMysql.GetErrorMsg());
        return;
    }

    // execute
    FORLIST(sqls, i)
    {
        const char* strSql = sqls[i].c_str();
        mMysql.Execute(strSql);
        if(mMysql.GetErrorCode() != 0)
        {
            LOG_ERROR("SQL execute occuried errors, sql=[%s], error=[%u], des=[%s]", strSql, mMysql.GetErrorCode(), mMysql.GetErrorMsg());
            mMysql.Execute("ROOLBACK;");
            return;
        }

        LOG_DEBUG("SQL = [%s]", strSql);
    }

    // commit
    mMysql.Execute("COMMIT;");
    if(mMysql.GetErrorCode() != 0)
    {
        LOG_ERROR("SQL commit occuried errors, error=[%u], des=[%s]", mMysql.GetErrorCode(), mMysql.GetErrorMsg());
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

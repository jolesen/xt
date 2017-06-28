#include "common/core/log.h"
#include "common/data/modules.h"
#include "common/data/value.h"
#include "common/msg/user.h"
#include "dbio.h"

bool Query(CMysql &mysql, uint max, uint uid, UIntList &list, const char* table)
{
    if(max > kValueIntColumnsLimit)
    {
        LOG_ERROR("max=%u > limit=%u", max, kValueIntColumnsLimit);
        return false;
    }

    std::ostringstream stream;
    stream << "SELECT ";
    char strTemp[64] = { 0 };
    FOR(max, i)
    {
        snprintf(strTemp, sizeof(strTemp), (i != max - 1) ? "`v%u`," : "`v%u`", i);
        stream << strTemp;
    }
    snprintf(strTemp, sizeof(strTemp), " FROM `%s` WHERE `uid` = %u;", table, uid);
    stream << strTemp;

    // QUERY
    QUERY(stream.str().c_str())
    {
        FOR(max, i)
        {
            list.push_back(mysql.GetUInt32(col++));
        }
    }

    return true;
}

void Save(StringList &sqls, uint uid, const UIntList &list, const char* table)
{
    if(list.size() > kValueIntColumnsLimit)
    {
        LOG_ERROR("list.size()=%u > limit=%u", list.size(), kValueIntColumnsLimit);
        return;
    }

    // SAVE
    PREPARE;
    APPEND("DELETE FROM `%s` WHERE `uid` = %u;", table, uid);
    if(list.size() > 0)
    {
        std::ostringstream stream;
        std::ostringstream streamParam;
        std::ostringstream streamValue;
        char strTemp[64] = { 0 };

        snprintf(strTemp, sizeof(strTemp), "INSERT INTO `%s`(`uid`, ", table);
        stream << strTemp;

        FORLIST(list, i)
        {
            if(i != list.size() - 1)
            {
                snprintf(strTemp, sizeof(strTemp), "`v%u`,", i);
                streamParam << strTemp;

                snprintf(strTemp, sizeof(strTemp), "%u,", list[i]);
                streamValue << strTemp;
            }
            else
            {
                snprintf(strTemp, sizeof(strTemp), "`v%u`) VALUE(%u,", i, uid);
                streamParam << strTemp;

                snprintf(strTemp, sizeof(strTemp), "%u);", list[i]);
                streamValue << strTemp;
            }
        }

        stream << streamParam.str().c_str() << streamValue.str().c_str();

        APPEND(stream.str().c_str());
    }
}

// ---------------------------------------------------------
// kModuleVar
LOAD_FUNC(kModuleVar)
{
    return Query(mysql, kValueVarMax, user.base.id, user.vars.list, "ints_var");
}

SAVE_FUNC(kModuleVar, vars)
{
    Save(sqls, user.base.id, user.vars.list, "ints_var");
}

// kModuleDailyReset
LOAD_FUNC(kModuleDailyReset)
{
    return Query(mysql, kValueDailyResetMax, user.base.id, user.dailyResets.list, "ints_daily");
}

SAVE_FUNC(kModuleDailyReset, dailyResets)
{
    Save(sqls, user.base.id, user.dailyResets.list, "ints_daily");
}

// kModuleTime
LOAD_FUNC(kModuleTime)
{
    return Query(mysql, kValueTimeMax, user.base.id, user.times.list, "ints_time");
}

SAVE_FUNC(kModuleTime, times)
{
    Save(sqls, user.base.id, user.times.list, "ints_time");
}

// kModuleCount
LOAD_FUNC(kModuleCount)
{
    return Query(mysql, kValueTimeMax, user.base.id, user.counts.list, "ints_count");
}

SAVE_FUNC(kModuleCount, counts)
{
    Save(sqls, user.base.id, user.counts.list, "ints_count");
}

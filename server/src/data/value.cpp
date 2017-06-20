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
    char str_temp[64] = { 0 };
    FOR(max, i)
    {
        snprintf(str_temp, sizeof(str_temp), (i != max - 1) ? "`v%u`," : "`v%u`", i);
        stream << str_temp;
    }
    snprintf(str_temp, sizeof(str_temp), " FROM `%s` WHERE `uid` = %u;", table, uid);
    stream << str_temp;

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
        std::ostringstream stream_param;
        std::ostringstream stream_value;
        char str_temp[64] = { 0 };

        snprintf(str_temp, sizeof(str_temp), "INSERT INTO `%s`(`uid`, ", table);
        stream << str_temp;

        FORLIST(list, i)
        {
            if(i != list.size() - 1)
            {
                snprintf(str_temp, sizeof(str_temp), "`v%u`,", i);
                stream_param << str_temp;

                snprintf(str_temp, sizeof(str_temp), "%u,", list[i]);
                stream_value << str_temp;
            }
            else
            {
                snprintf(str_temp, sizeof(str_temp), "`v%u`) VALUE(%u,", i, uid);
                stream_param << str_temp;

                snprintf(str_temp, sizeof(str_temp), "%u);", list[i]);
                stream_value << str_temp;
            }
        }

        stream << stream_param.str().c_str() << stream_value.str().c_str();

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
    return Query(mysql, kValueDailyResetMax, user.base.id, user.daily_resets.list, "ints_daily");
}

SAVE_FUNC(kModuleDailyReset, daily_resets)
{
    Save(sqls, user.base.id, user.daily_resets.list, "ints_daily");
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

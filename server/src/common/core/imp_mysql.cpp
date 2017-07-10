#include <cstring>
#include "mysql.h"

CMysql::CMysql()
{
    mMysql     = NULL;
    mResult    = NULL;
    mRow       = NULL;
    mFields    = NULL;
    mConnected = NULL;
}

CMysql::~CMysql()
{
    Close();
}

bool CMysql::Connect(const SMysqlConfig &config)
{
    Close();

    mMysql = mysql_init(NULL);
    if(!mMysql)
    {
        return false;
    }

    MYSQL *temp = mysql_real_connect(mMysql, config.host.c_str(), config.usr.c_str(), config.pwd.c_str(),
            config.db.c_str(), config.port, NULL, CLIENT_FOUND_ROWS | CLIENT_MULTI_RESULTS);
    if(mMysql == temp)
    {
        mysql_set_character_set(mMysql, "utf8");
        mConnected = true;
    }

    return mConnected;
}

void CMysql::Close()
{
    FreeResult();

    if(mMysql)
    {
        mysql_close(mMysql);
        mMysql = NULL;
    }

    mConnected = false;
}

bool CMysql::TestConnect()
{
    if(IsConnected())
    {
        return (mysql_ping(mMysql) == 0);
    }

    return false;
}

uint CMysql::Query(const char *sql)
{
    if(!sql)
    {
        return 0;
    }

    FreeResult();

    if(mysql_real_query(mMysql, sql, strlen(sql)))
    {
        return 0;
    }

    uint rows = 0;
    mResult = mysql_store_result(mMysql);
    if(mResult)
    {
        rows = (uint)mysql_num_rows(mResult);
    }

    return rows;
}

uint CMysql::Execute(const char* sql)
{
    if(!sql)
    {
        return 0;
    }

    FreeResult();

    if(mysql_real_query(mMysql, sql, strlen(sql)))
    {
        return 0;
    }

    uint rows = (uint)mysql_affected_rows(mMysql);
    return rows;
}

void CMysql::First()
{
    if(mResult)
    {
        mysql_data_seek(mResult, 0);
        mysql_field_seek(mResult, 0);

        Next();
    }
}

void CMysql::Next()
{
    if(mResult)
    {
        mRow = mysql_fetch_row(mResult);
        mFields = mysql_fetch_lengths(mResult);
    }
}

bool CMysql::Empty()
{
    return (!mResult || !mRow || !(*mRow) || !mFields);
}

uint CMysql::GetUInt32(uint index)
{
    if(!mResult)
    {
        return 0;
    }
    if(index >= (uint)mResult->field_count)
    {
        return 0;
    }
    if(mFields[index] <= 0)
    {
        return 0;
    }

    char buff[16] = { 0 };
    uint size = sizeof(buff) - 1;
    if(mFields[index] > size)
    {
        size = mFields[index];
    }
    memcpy(buff, mRow[index], size);

    char *begin = buff;
    char *end   = begin + sizeof(buff) - 1;

    if(*begin == '\0')
    {
        return 0;
    }

    for(begin = begin + 1; (*begin != '\0') && (begin < end); ++begin)
    {
        if(*begin < '0' || *begin > '9')
        {
            return 0;
        }
    }

    return atoi(buff);
}

ulong CMysql::GetULong(uint index)
{
    if(!mResult)
    {
        return 0;
    }
    if(index >= (uint)mResult->field_count )
    {
        return 0;
    }
    if(mFields[index] <= 0)
    {
        return 0;
    }

    char buff[32] = { 0 };
    uint size = sizeof(buff) - 1;
    if(mFields[index] > size)
    {
        size = mFields[index];
    }
    memcpy(buff, mRow[index], size);

    char *begin = buff;
    char *end   = begin + sizeof(buff) - 1;

    if(*begin == '\0')
    {
        return 0;
    }

    for(begin = begin + 1; (*begin != '\0') && (begin < end); ++begin)
    {
        if(*begin < '0' || *begin > '9')
        {
            return 0;
        }
    }

    return atoll(buff);
}

string CMysql::GetString(uint index)
{
    if(!mResult)
    {
        return string();
    }

    if(index >= (uint)mResult->field_count)
    {
        return string();
    }

    if(mFields[index] <= 0)
    {
        return string();
    }

    return string((char*)mRow[index], mFields[index]);
}

ulong CMysql::GetInsertId()
{
    ulong id = 0;
    if(mMysql)
    {
        id = (ulong)mysql_insert_id(mMysql);
    }

    return id;
}

uint CMysql::GetErrorCode()
{
    uint code = 0;
    if(mMysql)
    {
        code = mysql_errno(mMysql);
    }

    return code;
}

const char* CMysql::GetErrorMsg()
{
    if(mMysql)
    {
        return mysql_error(mMysql);
    }

    return "";
}

string CMysql::Escape(const char *strValue)
{
    return Escape(strValue, (uint)strlen(strValue));
}

string CMysql::Escape(const void *ptr, uint size)
{
    if(!mMysql || !ptr || size <= 0)
    {
        return string();
    }

    string buff(size * 4 + 1, '\0');

    mysql_real_escape_string(mMysql, (char*)buff.c_str(), (char*)ptr, size);

    return buff;
}

string CMysql::Escape(string strValue)
{
    return Escape(strValue.c_str(), strValue.size());
}

void CMysql::FreeResult()
{
    if(!mMysql)
    {
        return;
    }

    if(mResult)
    {
        mysql_free_result(mResult);
    }
    else
    {
        mResult = mysql_store_result(mMysql);
    }

    while(mysql_next_result(mMysql) == 0)
    {
        mResult = mysql_store_result(mMysql);
        if(mResult)
        {
            mysql_free_result(mResult);
        }
    }

    mResult = NULL;
}

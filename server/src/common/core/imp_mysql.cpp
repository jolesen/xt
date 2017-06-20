#include <cstring>
#include "mysql.h"

CMysql::CMysql()
{
    m_mysql     = NULL;
    m_result    = NULL;
    m_row       = NULL;
    m_fields    = NULL;
    m_connected = NULL;
}

CMysql::~CMysql()
{
    Close();
}

bool CMysql::Connect(const SMysqlConfig &config)
{
    Close();

    m_mysql = mysql_init(NULL);
    if(!m_mysql)
    {
        return false;
    }

    MYSQL *temp = mysql_real_connect(m_mysql, config.host.c_str(), config.usr.c_str(), config.pwd.c_str(),
            config.db.c_str(), config.port, NULL, CLIENT_FOUND_ROWS | CLIENT_MULTI_RESULTS);
    if(m_mysql == temp)
    {
        mysql_set_character_set(m_mysql, "utf8");
        m_connected = true;
    }

    return m_connected;
}

void CMysql::Close()
{
    FreeResult();

    if(m_mysql)
    {
        mysql_close(m_mysql);
        m_mysql = NULL;
    }

    m_connected = false;
}

bool CMysql::TestConnect()
{
    if(IsConnected())
    {
        return (mysql_ping(m_mysql) == 0);
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

    if(mysql_real_query(m_mysql, sql, strlen(sql)))
    {
        return 0;
    }

    uint rows = 0;
    m_result = mysql_store_result(m_mysql);
    if(m_result)
    {
        rows = (uint)mysql_num_rows(m_result);
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

    if(mysql_real_query(m_mysql, sql, strlen(sql)))
    {
        return 0;
    }

    uint rows = (uint)mysql_affected_rows(m_mysql);
    return rows;
}

void CMysql::First()
{
    if(m_result)
    {
        mysql_data_seek(m_result, 0);
        mysql_field_seek(m_result, 0);

        Next();
    }
}

void CMysql::Next()
{
    if(m_result)
    {
        m_row = mysql_fetch_row(m_result);
        m_fields = mysql_fetch_lengths(m_result);
    }
}

bool CMysql::Empty()
{
    return (!m_result || !m_row || !(*m_row) || !m_fields);
}

uint CMysql::GetUInt32(uint index)
{
    if(!m_result)
    {
        return 0;
    }
    if(index >= (uint)m_result->field_count)
    {
        return 0;
    }
    if(m_fields[index] <= 0)
    {
        return 0;
    }

    char buff[16] = { 0 };
    uint size = sizeof(buff) - 1;
    if(m_fields[index] > size)
    {
        size = m_fields[index];
    }
    memcpy(buff, m_row[index], size);

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
    if(!m_result)
    {
        return 0;
    }
    if(index >= (uint)m_result->field_count )
    {
        return 0;
    }
    if(m_fields[index] <= 0)
    {
        return 0;
    }

    char buff[32] = { 0 };
    uint size = sizeof(buff) - 1;
    if(m_fields[index] > size)
    {
        size = m_fields[index];
    }
    memcpy(buff, m_row[index], size);

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

std::string CMysql::GetString(uint index)
{
    if(!m_result)
    {
        return std::string();
    }

    if(index >= (uint)m_result->field_count)
    {
        return std::string();
    }

    if(m_fields[index] <= 0)
    {
        return std::string();
    }

    return std::string((char*)m_row[index], m_fields[index]);
}

ulong CMysql::GetInsertId()
{
    ulong id = 0;
    if(m_mysql)
    {
        id = (ulong)mysql_insert_id(m_mysql);
    }

    return id;
}

uint CMysql::GetErrorCode()
{
    uint code = 0;
    if(m_mysql)
    {
        code = mysql_errno(m_mysql);
    }

    return code;
}

const char* CMysql::GetErrorMsg()
{
    if(m_mysql)
    {
        return mysql_error(m_mysql);
    }

    return "";
}

std::string CMysql::Escape(const char *strValue)
{
    return Escape(strValue, (uint)strlen(strValue));
}

std::string CMysql::Escape(const void *ptr, uint size)
{
    if(!m_mysql || !ptr || size <= 0)
    {
        return std::string();
    }

    std::string buff(size * 4 + 1, '\0');

    mysql_real_escape_string(m_mysql, (char*)buff.c_str(), (char*)ptr, size);

    return buff;
}

std::string CMysql::Escape(std::string strValue)
{
    return Escape(strValue.c_str(), strValue.size());
}

void CMysql::FreeResult()
{
    if(!m_mysql)
    {
        return;
    }

    if(m_result)
    {
        mysql_free_result(m_result);
    }
    else
    {
        m_result = mysql_store_result(m_mysql);
    }

    while(mysql_next_result(m_mysql) == 0)
    {
        m_result = mysql_store_result(m_mysql);
        if(m_result)
        {
            mysql_free_result(m_result);
        }
    }

    m_result = NULL;
}

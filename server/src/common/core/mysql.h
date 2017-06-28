#ifndef _COMMON_CORE_MYSQL_H_
#define _COMMON_CORE_MYSQL_H_

#include <mysql.h>
#include "core.h"

// SMysqlConfig
struct SMysqlConfig
{
    std::string host;
    std::string db;
    std::string usr;
    std::string pwd;
    uint        port;
};

// CMysql
class CMysql
{
public:
    CMysql();
    ~CMysql();

    bool Connect(const SMysqlConfig &cfg);
    void Close();
    bool TestConnect();
    uint Query(const char *sql); // return queried  rows
    uint Execute(const char *sql); // return affected rows
    void First();
    void Next();
    bool Empty();

    uint        GetUInt32(uint index);
    ulong       GetULong(uint index);
    std::string GetString(uint index);
    ulong       GetInsertId();
    uint        GetErrorCode();
    const char* GetErrorMsg();
    std::string Escape(const char *str);
    std::string Escape(const void *ptr, uint size);
    std::string Escape(std::string str);

private:
    void FreeResult();
    bool IsConnected() { return mConnected; }

private:
    MYSQL*         mMysql;
    MYSQL_RES*     mResult;
    MYSQL_ROW      mRow;
    unsigned long* mFields;
    bool           mConnected;
};

#endif

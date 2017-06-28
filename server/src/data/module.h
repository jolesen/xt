#ifndef _DATA_MODULE_H_
#define _DATA_MODULE_H_

#include "common/msg/user.h"

#define WORKERS_COUNT 2

// SModule
struct SModule
{
    typedef bool (*FunModuleLoader )(SUser &user, CMysql &mysql);
    typedef void (*FunModuleDecoder)(SUser &user, CCoder &coder);
    typedef void (*FunModuleSaver  )(const SUser &user, StringList &sqls);

    std::map<uint, FunModuleLoader>  loaders;
    std::map<uint, FunModuleDecoder> decoders;
    std::map<uint, FunModuleSaver>   savers;

    // -----------------------
    bool Register(uint module, FunModuleLoader funLoader, FunModuleDecoder funCoder, FunModuleSaver funSaver)
    {
        if(funLoader)
        {
            loaders[module] = funLoader;
        }

        if(funCoder)
        {
            decoders[module] = funCoder;
        }

        if(funSaver)
        {
            savers[module] = funSaver;
        }

        return 0;
    }
};
#define theModule CSingleton<SModule>::Instance()

// ------------- LOAD ---------------
// LOAD_FUNC (M - module)
#define LOAD_FUNC(M)\
    bool LoadModule_##M(SUser &user, CMysql &mysql);\
    bool gRegLoader_##M = theModule.Register(M, LoadModule_##M, NULL, NULL);\
    bool LoadModule_##M(SUser &user, CMysql &mysql)

// QUERY (fmt - sql, ... - params)
#define QUERY(fmt, ...)\
    char strSql[KB / 2] = { 0 };\
    snprintf(strSql, sizeof(strSql), fmt, ##__VA_ARGS__);\
    uint rows = mysql.Query(strSql);\
    LOG_DEBUG("SQL = [%s]", strSql);\
    if((rows == 0) && (mysql.GetErrorCode() != 0))\
    {\
        LOG_ERROR("SQL query occuried errors, sql=[%s], error=[%u], des=[%s]", strSql, mysql.GetErrorCode(), mysql.GetErrorMsg());\
        return false;\
    }\
    uint col = 0;\
    for(mysql.First(); !mysql.Empty(); mysql.Next(), col = 0)\


// ------------- SAVE ---------------
// SAVE_FUNC (M - module, T - data type)
#define SAVE_FUNC(M, T)\
    void DecodeModule_##M(SUser &user, CCoder &coder) { user.T.Decode(coder); };\
    void SaveModule_##M(const SUser &user, StringList &sqls);\
    bool gRegSaver_##M = theModule.Register(M, NULL, DecodeModule_##M, SaveModule_##M);\
    void SaveModule_##M(const SUser &user, StringList &sqls)

// PREPARE
#define PREPARE\
    char strAppend[KB] = { 0 }

// APPEND (fmt - sql, ... - params)
#define APPEND(fmt, ...)\
    snprintf(strAppend, sizeof(strAppend), fmt, ##__VA_ARGS__);\
    sqls.push_back(strAppend)

#endif

#ifndef _COMMON_CORE_REDIS_H_
#define _COMMON_CORE_REDIS_H_

#include "core.h"

// CRedis
class CRedis
{
public:
    bool Ping();

private:
    friend CRedis& CSingleton<CRedis>::Instance();
    CRedis();
    ~CRedis();
};
#define theRedis CSingleton<CRedis>::Instance()

#endif

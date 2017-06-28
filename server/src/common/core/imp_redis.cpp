#include <hiredis.h>
#include "sconfig.h"
#include "redis.h"
#include "log.h"

CRedis::CRedis()
{
}

CRedis::~CRedis()
{
}

bool CRedis::Ping()
{
    const SHost &host = theServerConfig.redis_host;
    redisContext* context = redisConnect(host.ip.c_str(), host.port);
    if(!context)
    {
        LOG_ERROR("can not connect to redis server[%s:%u]", host.ip.c_str(), host.port);
        return false;
    }

    if(context->err)
    {
        LOG_ERROR("connect to redis server[%s:%u], error:%s", host.ip.c_str(), host.port, context->errstr);
        redisFree(context);
        return false;
    }

    return true;
}

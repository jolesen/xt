#include "common/core/sconfig.h"
#include "common/core/signal.h"
#include "common/core/mongo.h"
#include "common/core/redis.h"
#include "common/core/log.h"
#include "test.h"

void TestMongo()
{
    theServerConfig.mongo_host = SHost("127.0.0.1", 27017);
    theMongo.Ping();
    StringList list;
    theMongo.Query(list, "mydb", "user");
    FORLIST(list, i)
    {
        LOG_INFO("%s", list[i].c_str());
    }
}

void TestRedis()
{
    theServerConfig.redis_host = SHost("127.0.0.1", 6379);
    if(theRedis.Ping())
    {
        LOG_INFO("ping redis server ok");
    }
    else
    {
        LOG_INFO("ping redis server FAILED");
    }
}

int main(int argc, const char **argv)
{
    theLog.SetFileName("test.log");

    //theSignal.Init();
    //TestCoder();
    //TestThread();
    TestMongo();
    TestRedis();

    return 0;
}

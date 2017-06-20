#include "common/core/sconfig.h"
#include "common/core/signal.h"
#include "common/core/mongo.h"
#include "common/core/log.h"
#include "test.h"

int main(int argc, const char **argv)
{
    theLog.SetFileName("test.log");

    //theSignal.Init();
    //TestCoder();
    //TestThread();

    theServerConfig.mongo_host = SHost("127.0.0.1", 27017);
    theMongo.Ping();
    StringList list;
    theMongo.Query(list, "mydb", "user");
    FORLIST(list, i)
    {
        LOG_INFO("%s", list[i].c_str());
    }

    return 0;
}

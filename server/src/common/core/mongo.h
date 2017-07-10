#ifndef _COMMON_CORE_MONGO_H_
#define _COMMON_CORE_MONGO_H_

#include <mongoc.h>
#include "core.h"

// CMongo
class CMongo
{
public:
    bool Ping();
    uint Query(StringList &listRet, const string &db, const string &coll);

private:
    friend CMongo& CSingleton<CMongo>::Instance();
    CMongo();
    ~CMongo();
};
#define theMongo CSingleton<CMongo>::Instance()

#endif

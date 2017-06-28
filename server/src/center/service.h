#ifndef _LOGIC_SERVICE_H_
#define _LOGIC_SERVICE_H_

#include "common/msg/system.h"

// CServiceManager
class CServiceManager
{
private:
    friend CServiceManager& CSingleton<CServiceManager>::Instance();
    CServiceManager();
    ~CServiceManager();

public:
    typedef std::map<std::string, SService> StringServiceMap;

    const StringServiceMap& GetServices() { return mService; }
    void AddService(const SService &service) { mService[service.sid] = service; }

private:
    StringServiceMap mService;
};
#define theServiceManager CSingleton<CServiceManager>::Instance()

#endif

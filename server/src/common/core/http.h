#ifndef _COMMON_CORE_HTTP_H_
#define _COMMON_CORE_HTTP_H_

#include <event2/http.h>
#include "core.h"

// CHttpServer
class CHttpServer
{
public:
    CHttpServer();
    ~CHttpServer();

    uint Create(event_base* base);
    void Free();

private:
    static void OnHttpRequest(evhttp_request* request, void* args);

private:
    evhttp* mHttp;
};

#endif

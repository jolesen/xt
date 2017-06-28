#include <event2/buffer.h>
#include "errorcode.h"
#include "sconfig.h"
#include "monitor.h"
#include "http.h"
#include "log.h"

CHttpServer::CHttpServer()
{
}

CHttpServer::~CHttpServer()
{
    Free();
}

void CHttpServer::Free()
{
    if(mHttp)
    {
        evhttp_free(mHttp);
    }
}

uint CHttpServer::Create(event_base* base)
{
    const SHost  &host = theServerConfig.httpHost;
    if((host.ip == "") || !host.port)
    {
        return 0;
    }

    mHttp = evhttp_new(base);
    if(!mHttp)
    {
        return ERR_CORE_SERVER_HTTP_HTTP_NEW;
    }

    // bind
    uint ret = evhttp_bind_socket(mHttp, host.ip.c_str(), host.port);
    if(ret)
    {
        return ERR_CORE_SERVER_HTTP_BIND_SOCKET;
    }

    // callback
    evhttp_set_gencb(mHttp, CHttpServer::OnHttpRequest, NULL);

    return 0;
}

void CHttpServer::OnHttpRequest(evhttp_request* request, void* args)
{
    const evhttp_uri *arrURI = evhttp_request_get_evhttp_uri(request);
    const char* host = evhttp_uri_get_host(arrURI);
    int port = evhttp_uri_get_port(arrURI);

    const char* uri = evhttp_request_get_uri(request);
    MONITOR(OnHttpRequest(uri));
    LOG_INFO("get a http request[%s:%d], uri:%s", host, port, uri);

    evbuffer* buff = evbuffer_new();
    if(!buff)
    {
        return;
    }

    evbuffer_add_printf(buff, "<html>");
    evbuffer_add_printf(buff, "<head><title>MyHttpServer</title></head>");
    evbuffer_add_printf(buff, "<body>");
    evbuffer_add_printf(buff, "<p>Welcome to my http server</p>");
    evbuffer_add_printf(buff, "</body>");
    evbuffer_add_printf(buff, "</html>\n");

    // rsp
    evhttp_send_reply(request, HTTP_OK, "OK", buff);
    evbuffer_free(buff);
}

#include "common/core/server.h"
#include "common/core/linker.h"
#include "common/core/timer.h"
#include "common/core/log.h"
#include "common/core/time.h"
#include "common/msg/system.h"
#include "common/msg/test.h"
#include "common/misc.h"

// ----------------------------------------------------------------------------

#define TEST_AMOUNT 1

#define if_notfind(M, K) typeof((M).begin()) TEMP_VAR = (M).find(K); if(TEMP_VAR == (M).end())
//#define forlist(L, V) for(typeof((L).begin()) TEMP_VAR = (L).begin(), typeof((L).begin()) V; TEMP_VAR != (L).end(), V = *TEMP_VAR; ++TEMP_VAR)

//#define IF_FIND(M, K, ITER)      typeof((M).begin()) ITER = (M).find(K); if(ITER != (M).end())
//#define IF_NOTFIND(M, K, ITER)   typeof((M).begin()) ITER = (M).find(K); if(ITER == (M).end())



void _test_marco()
{
    std::map<std::string, std::string> mpIdName;

    if_notfind(mpIdName, "notfind")
    {
    }

    FORMAP(mpIdName, iter)
    {
    }
}

uint g_seq = 1;
bool CheckInnerClient(const STimerInfo &info)
{
    theLinker.CheckLink();

    FOR(TEST_AMOUNT, i)
    {
        //CClient *client = theServer.GetLinkout(i);
        CClient *client = theLinker.GetLink(""); // TODO
        if(!client)
        {
            LOG_ERROR("cannot get linkout=%u", i);
            continue;
        }

        if(client->GetState() == kClientStateConnected)
        {
            QTestTime req;
            //req.header.src = 0;
            //req.header.dst = 2;
            req.time = CTime::Now();
            req.seq  = g_seq++;
            client->Write(&req);
            LOG_INFO("---------send msg, time:%u, seq:%u", req.time, req.seq);
        }
    }

    return true;
}

EVENT_FUNC(SEventLinkConnected)
{
    CMisc::OnConnected(ev.client);
}

MSG_FUNC(RTestTime)
{
    LOG_INFO("get msg, time:%u, seq:%u", msg.time, msg.seq);
}

bool Init()
{
    theTimer.AddSecond(1, 0, CheckInnerClient);
    FOR(TEST_AMOUNT, i)
    {
        SHost host("127.0.0.1", 5000);
        theLinker.Link("gate_1", host);
    }

    return true;
}

// ----------------------------------------------------------------------------
int main(int argc, const char **argv)
{
    return CMisc::Main(argc, argv, NULL, Init);
}

#include <arpa/inet.h>
#include <algorithm>
#include <numeric>
#include <errno.h>
#include "sconfig.h"
#include "time.h"
#include "util.h"

CUtil::CUtil()
{
}

CUtil::~CUtil()
{
}

bool CUtil::SleepMillisecond(uint ms)
{
    if(!ms)
    {
        return true;
    }

    timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    if(nanosleep(&ts, &ts) == -1)
    {
        if(errno != EINTR) // Interrupted by a signal, Ingore.
        {
            return false;
        }
    }

    return true;
}

bool CUtil::StartupAsDaemon()
{
    if(daemon(1, 0) == -1)
    {
        return false;
    }

    return true;
}

uint CUtil::ToInt(const std::string &value)
{
    if(value != "")
    {
        return std::atoi(value.c_str());
    }

    return 0;
}

std::string CUtil::ToString(uint value)
{
    char strTemp[64] = { 0 };
    snprintf(strTemp, sizeof(strTemp), "%u", value);
    return std::string(strTemp);
}

std::string CUtil::GetIpString(uint ip)
{
    in_addr addr;
    addr.s_addr = ip;
    return std::string(inet_ntoa(addr));
}

uint CUtil::GetIpInt(const std::string &ip)
{
    return (uint)inet_addr(ip.c_str());
}

StringList CUtil::SplitString(const std::string &strBase, const std::string &strSpliter)
{
    StringList result;
    if(strBase == "")
    {
        return result;
    }

    if(strBase.find(strSpliter) == std::string::npos)
    {
        result.push_back(strBase);
        return result;
    }

    uint fromIndex      = 0;
    size_t spliterIndex = 0;
    while((spliterIndex = strBase.find(strSpliter, fromIndex)) != std::string::npos)
    {
        result.push_back(strBase.substr(fromIndex, spliterIndex));

        fromIndex = spliterIndex + 1;
    }

    // last one
    if(fromIndex <= (uint)(strBase.size() - 1))
    {
        result.push_back(strBase.substr(fromIndex, strBase.size() - 1));
    }

    return result;
}

UIntList CUtil::SplitInt(const std::string &strBase, const std::string &strSpliter)
{
    UIntList result;
    StringList list = SplitString(strBase, strSpliter);
    FORLIST(list, i)
    {
        uint value = ToInt(list[i]);
        result.push_back(value);
    }

    return result;
}

void CUtil::MergeList(S2UIntList &list, const S2UIntList &added)
{
    bool found = false;
    FORLIST(added, i)
    {
        found = false;
        FORLIST(list, j)
        {
            if(added[i].id == list[j].id)
            {
                list[j].value += added[i].value;
                found = true;
                break;
            }
        }
        if(!found)
        {
            list.push_back(added[i]);
        }
    }
}

void CUtil::MergeList(S3UIntList &list, const S3UIntList &added)
{
    bool found = false;
    FORLIST(added, i)
    {
        found = false;
        FORLIST(list, j)
        {
            if((added[i].type == list[j].type) && (added[i].id == list[j].id))
            {
                list[j].value += added[i].value;
                found = true;
                break;
            }
        }
        if(!found)
        {
            list.push_back(added[i]);
        }
    }
}

void CUtil::Trim(std::string &strBase)
{
    strBase.erase(0, strBase.find_first_not_of(" \t\n\r"));
    strBase.erase(strBase.find_last_not_of(" \t\n\r") + 1);
}

void CUtil::Replace(std::string &strBase, const std::string &strOld, const std::string &strNew)
{
    std::string::size_type pos = 0;
    std::string::size_type oldLen = strOld.size();
    std::string::size_type newLen = strNew.size();
    pos = strBase.find(strOld, pos);
    while((pos != std::string::npos))
    {
        strBase.replace(pos, oldLen, strNew);
        pos = strBase.find(strOld, (pos + newLen));
    }
}

uint _GetInitSeed()
{
    // 避免同时启动的进程产生的随机数序列都一样
    uint sum = CTime::Now();
    FOR(theServerConfig.sid.size(), i)
    {
        sum += (uint)theServerConfig.sid.at(i);
    }
    return sum;
}

uint CUtil::Rand(uint min, uint max)
{
    static ulong seed = _GetInitSeed();
    srand(++seed);
    return min + (uint)(rand() % (max - min + 1));
}

uint CUtil::RandIndex(UIntList probs, uint size)
{
    uint total = 0;
    FORLIST(probs, i)
    {
        total += probs[i];
    }

    uint rand = Rand(0, total);
    FOR(size, i)
    {
        if(probs[i] >= rand)
        {
            return i;
        }
        else
        {
            rand -= probs[i];
        }
    }

    return 0;
}

bool CUtil::RandMany(UIntList &listResult, uint min, uint max, uint count, bool isDiff)
{
    if(max < min)
    {
        return false;
    }

    if(isDiff)
    {
        if(min + count > max)
        {
            return false;
        }

        UIntList list;
        for(uint i = min; i < max; ++i)
        {
            list.push_back(i);
        }
        std::random_shuffle(list.begin(), list.end());

        FOR(count, i)
        {
            listResult.push_back(list[i]);
        }
    }
    else
    {
        FOR(count, i)
        {
            uint rand = Rand(min, max);
            listResult.push_back(rand);
        }
    }

    return true;
}

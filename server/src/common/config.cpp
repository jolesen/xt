#include "core/util.h"
#include "core/log.h"
#include "config.h"

// -----------------------------------------------
// 脚本启动参数字段
const string kArgsSid    = "-sid";
const string kArgsConfig = "-config";

// 启动配置文件参数字段
const string kConfigHost         = "Host";
const string kConfigHttpHost     = "HttpHost";
const string kConfigCenter       = "Center";
const string kConfigClientLimit  = "ClientLimit";
const string kConfigBacklog      = "Backlog";
const string kConfigResource     = "Resource";
const string kConfigBroadcastMax = "BroadcastMax";
const string kConfigMonitorTick  = "MonitorTick";
const string kConfigMongoHost    = "MongoHost";
const string kConfigRedisHost    = "RedisHost";

// -----------------------------------------------
string _ParseHost(SHost &host, const string &value)
{
    StringList list = CUtil::SplitString(value, ":");
    if(list.size() == 2)
    {
        host.ip   = list[0];
        host.port = CUtil::ToInt(list[1]);
        return "";
    }

    char buff[KB] = { 0 };
    snprintf(buff, sizeof(buff), "[%s], error host", value.c_str());
    return string(buff);
}

string _ParseField(const string &strLine)
{
    string flag = "";
    StringList list = CUtil::SplitString(strLine, "=");
    if(list.size() == 2)
    {
        string key   = list[0];
        string value = list[1];

        if(key == kConfigHost)
        {
            flag = _ParseHost(theConfig.host, value);
        }
        else if(key == kConfigHttpHost)
        {
            flag = _ParseHost(theConfig.httpHost, value);
        }
        else if(key == kConfigMongoHost)
        {
            flag = _ParseHost(theConfig.mongoHost, value);
        }
        else if(key == kConfigRedisHost)
        {
            flag = _ParseHost(theConfig.redisHost, value);
        }
        else if(key == kConfigCenter)
        {
            StringList temp = CUtil::SplitString(value, ",");
            if(temp.size() != 2)
            {
                char buff[KB] = { 0 };
                snprintf(buff, sizeof(buff), "[%s], error center config", value.c_str());
                return string(buff);
            }

            SHost host;
            flag = _ParseHost(host, temp[1]);
            if(!flag.size())
            {
                theConfig.centers[temp[0]] = host;
            }
        }
        else if(key == kConfigClientLimit)
        {
            theConfig.clientLimit = CUtil::ToInt(value);
        }
        else if(key == kConfigBacklog)
        {
            theConfig.backlog = CUtil::ToInt(value);
        }
        else if(key == kConfigBroadcastMax)
        {
            theConfig.broadcastMax = CUtil::ToInt(value);
        }
        else if(key == kConfigMonitorTick)
        {
            theConfig.monitorTick = CUtil::ToInt(value);
        }
        else if(key == kConfigResource)
        {
            theConfig.resource = value;
        }
        else
        {
            char buff[KB] = { 0 };
            snprintf(buff, sizeof(buff), "unknow key:%s", key.c_str());
            flag = string(buff);
        }
    }

    return flag;
}

string _ReadConfig()
{
    FILE *fp = fopen(theConfig.config.c_str(), "r");
    if(!fp)
    {
        char buff[KB] = { 0 };
        snprintf(buff, sizeof(buff), "[%s], can't open file:%s", theConfig.sid.c_str(), theConfig.config.c_str());
        return string(buff);
    }

    string strType = "";
    std::map<string, StringList> mpServer;
    char line[KB] = { 0 };
    while(fgets(line, KB - 1, fp))
    {
        string strLine = line;
        CUtil::Trim(strLine);
        CUtil::ReplaceAllSpace(strLine);

        if((strLine == "") || (strLine.at(0) == '/'))
        {
            continue;
        }

        if(strLine.at(0) == '#')
        {
            strType = strLine.substr(1);
            IF_FIND(mpServer, strType, iter)
            {
                char buff[KB] = { 0 };
                snprintf(buff, sizeof(buff), "[%s], config error, duplicate define:[%s]", theConfig.sid.c_str(), strType.c_str());
                return string(buff);
            }
        }
        else
        {
            mpServer[strType].push_back(strLine);
        }
    }
    fclose(fp);

    StringList arrConfig;
    arrConfig.push_back("common");
    arrConfig.push_back(theConfig.sid);
    FORLIST(arrConfig, i)
    {
        IF_FIND(mpServer, arrConfig[i], iter)
        {
            FORLIST(iter->second, j)
            {
                string strLine = (iter->second)[j];
                string ret = _ParseField(strLine);
                if(ret.size())
                {
                    char buff[KB] = { 0 };
                    snprintf(buff, sizeof(buff), "line:[%s], [%s]", strLine.c_str(), ret.c_str());
                    return string(buff);
                }
            }
        }
    }

    return "";
}

// -----------------------------------------------
// ./auth -sid auth-1 --xt
string CConfig::Parse(uint argc, const char **argv)
{
    --argc; // 不处理"--xt"
    FOR(argc, i)
    {
        if(i == 0)
        {
            string str = argv[i];
            theConfig.type = str.substr(2, str.length());
            continue;
        }

        string key = argv[i];
        if(i >= argc - 1)
        {
            char buff[KB] = { 0 };
            snprintf(buff, sizeof(buff), "[%s] no enough args", key.c_str());
            return string(buff);
        }

        string value = argv[++i];
        if(key == kArgsSid)
        {
            theConfig.sid = value;
        }
        else if(key == kArgsConfig)
        {
            theConfig.config = value;
        }
    }

    // 读取配置文件
    return _ReadConfig();
}

void CConfig::Show()
{
    LOG_INFO("----- START ARGS BEGIN -----");
    LOG_INFO("Type         : %s", type.c_str());
    LOG_INFO("SID          : %s", sid.c_str());
    LOG_INFO("Host         : %s:%u", host.ip.c_str(), host.port);
    LOG_INFO("HttpHost     : %s:%u", httpHost.ip.c_str(), httpHost.port);
    LOG_INFO("MongoHost    : %s:%u", mongoHost.ip.c_str(), mongoHost.port);
    LOG_INFO("RedisHost    : %s:%u", redisHost.ip.c_str(), redisHost.port);
    LOG_INFO("Resource     : %s", resource.c_str());
    LOG_INFO("ClientLimit  : %u", clientLimit);
    LOG_INFO("Backlog      : %u", backlog);
    LOG_INFO("BroadcastMax : %u", broadcastMax);
    LOG_INFO("MonitorTick  : %u", monitorTick);
    FORMAP(centers, iter)
    {
        LOG_INFO("Center       : %s, %s:%u", iter->first.c_str(), iter->second.ip.c_str(), iter->second.port);
    }
    LOG_INFO("----- START ARGS END   -----");
}

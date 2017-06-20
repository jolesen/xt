#include "core/util.h"
#include "core/log.h"
#include "config.h"

// -----------------------------------------------
// 脚本启动参数字段
const std::string kArgsSid    = "-sid";
const std::string kArgsConfig = "-config";

// 启动配置文件参数字段
const std::string kConfigHost         = "Host";
const std::string kConfigHttpHost     = "HttpHost";
const std::string kConfigCenter       = "Center";
const std::string kConfigClientLimit  = "ClientLimit";
const std::string kConfigBacklog      = "Backlog";
const std::string kConfigResource     = "Resource";
const std::string kConfigBroadcastMax = "BroadcastMax";
const std::string kConfigMonitorTick  = "MonitorTick";
const std::string kConfigMongoHost    = "MongoHost";

// -----------------------------------------------
std::string _ParseHost(SHost &host, const std::string &value)
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
    return std::string(buff);
}

std::string _ParseField(const std::string &strLine)
{
    std::string flag = "";
    StringList list = CUtil::SplitString(strLine, "=");
    if(list.size() == 2)
    {
        std::string key   = list[0];
        std::string value = list[1];

        if(key == kConfigHost)
        {
            flag = _ParseHost(theConfig.host, value);
        }
        else if(key == kConfigHttpHost)
        {
            flag = _ParseHost(theConfig.http_host, value);
        }
        else if(key == kConfigMongoHost)
        {
            flag = _ParseHost(theConfig.mongo_host, value);
        }
        else if(key == kConfigCenter)
        {
            StringList temp = CUtil::SplitString(value, ",");
            if(temp.size() != 2)
            {
                char buff[KB] = { 0 };
                snprintf(buff, sizeof(buff), "[%s], error center config", value.c_str());
                return std::string(buff);
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
            theConfig.client_limit = CUtil::ToInt(value);
        }
        else if(key == kConfigBacklog)
        {
            theConfig.backlog = CUtil::ToInt(value);
        }
        else if(key == kConfigBroadcastMax)
        {
            theConfig.broadcast_max = CUtil::ToInt(value);
        }
        else if(key == kConfigMonitorTick)
        {
            theConfig.monitor_tick = CUtil::ToInt(value);
        }
        else if(key == kConfigResource)
        {
            theConfig.resource = value;
        }
        else
        {
            char buff[KB] = { 0 };
            snprintf(buff, sizeof(buff), "unknow key:%s", key.c_str());
            flag = std::string(buff);
        }
    }

    return flag;
}

std::string _ReadConfig()
{
    FILE *fp = fopen(theConfig.config.c_str(), "r");
    if(!fp)
    {
        char buff[KB] = { 0 };
        snprintf(buff, sizeof(buff), "[%s], can't open file:%s", theConfig.sid.c_str(), theConfig.config.c_str());
        return std::string(buff);
    }

    std::string strType = "";
    std::map<std::string, StringList> mpServer;
    char line[KB] = { 0 };
    while(fgets(line, KB - 1, fp))
    {
        std::string strLine = line;
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
                return std::string(buff);
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
                std::string strLine = (iter->second)[j];
                std::string ret = _ParseField(strLine);
                if(ret.size())
                {
                    char buff[KB] = { 0 };
                    snprintf(buff, sizeof(buff), "line:[%s], [%s]", strLine.c_str(), ret.c_str());
                    return std::string(buff);
                }
            }
        }
    }

    return "";
}

// -----------------------------------------------
// ./auth -sid auth_1 --xt
std::string CConfig::Parse(uint argc, const char **argv)
{
    --argc; // 不处理"--xt"
    FOR(argc, i)
    {
        if(i == 0)
        {
            std::string str = argv[i];
            theConfig.type = str.substr(2, str.length());
            continue;
        }

        std::string key = argv[i];
        if(i >= argc - 1)
        {
            char buff[KB] = { 0 };
            snprintf(buff, sizeof(buff), "[%s] no enough args", key.c_str());
            return std::string(buff);
        }

        std::string value = argv[++i];
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
    LOG_INFO("HttpHost     : %s:%u", http_host.ip.c_str(), http_host.port);
    LOG_INFO("MongoHost    : %s:%u", mongo_host.ip.c_str(), mongo_host.port);
    LOG_INFO("Resource     : %s", resource.c_str());
    LOG_INFO("ClientLimit  : %u", client_limit);
    LOG_INFO("Backlog      : %u", backlog);
    LOG_INFO("BroadcastMax : %u", broadcast_max);
    LOG_INFO("MonitorTick  : %u", monitor_tick);
    FORMAP(centers, iter)
    {
        LOG_INFO("Center       : %s, %s:%u", iter->first.c_str(), iter->second.ip.c_str(), iter->second.port);
    }
    LOG_INFO("----- START ARGS END   -----");
}

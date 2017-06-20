#include "json.h"
#include "log.h"

CJson::CJson()
{
}

CJson::~CJson()
{
}

bool CJson::Read(const std::string &file)
{
    FILE *fp = fopen(file.c_str(), "r");
    if(!fp)
    {
        LOG_ERROR("\nCan't open file:%s\n", file.c_str());
        return false;
    }

    StringList list;
    char strLine[KB] = { 0 };
    while(fgets(strLine, KB - 1, fp))
    {
        list.push_back(strLine);
    }
    fclose(fp);

    return ReadString(list);
}

bool CJson::ReadString(const StringList &list)
{
    m_lines.clear();
    FORLIST(list, i)
    {
        const std::string line = list[i];
        if((line.find("{") == std::string::npos) || (line.find("}") == std::string::npos))
        {
            continue;
        }

        StringList list_value;
        std::size_t first = 0;
        while(true)
        {
            first = line.find("\"", first);
            if(first == std::string::npos)
            {
                break;
            }
            std::size_t second = line.find("\"", first + 1);
            if(second == std::string::npos)
            {
                break;
            }

            std::string strValue = line.substr(first + 1, second - (first + 1));
            list_value.push_back(strValue);

            first = second + 1;
        }

        if((list_value.size() % 2) != 0)
        {
            LOG_ERROR("\nerror line[%s], parsed size:%u", line.c_str(), (uint)list_value.size());
            return false;
        }

        CJsonLine jl;
        FORLIST(list_value, j)
        {
            std::string key   = list_value[j];
            std::string value = list_value[++j];
            jl.pairs[key] = value;
        }
        m_lines.push_back(jl);
    }

    return true;
}

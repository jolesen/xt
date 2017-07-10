#include "json.h"
#include "log.h"

CJson::CJson()
{
}

CJson::~CJson()
{
}

bool CJson::Read(const string &file)
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
    mLines.clear();
    FORLIST(list, i)
    {
        const string line = list[i];
        if((line.find("{") == string::npos) || (line.find("}") == string::npos))
        {
            continue;
        }

        StringList list_value;
        std::size_t first = 0;
        while(true)
        {
            first = line.find("\"", first);
            if(first == string::npos)
            {
                break;
            }
            std::size_t second = line.find("\"", first + 1);
            if(second == string::npos)
            {
                break;
            }

            string strValue = line.substr(first + 1, second - (first + 1));
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
            string key   = list_value[j];
            string value = list_value[++j];
            jl.pairs[key] = value;
        }
        mLines.push_back(jl);
    }

    return true;
}

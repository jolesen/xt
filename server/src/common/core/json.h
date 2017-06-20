#ifndef _COMMON_CORE_JSON_H_
#define _COMMON_CORE_JSON_H_

#include "core.h"

// CJson
class CJson
{
public:
    CJson();
    ~CJson();

    // CJsonLine
    struct CJsonLine
    {
        StringStringMap pairs;
    };

public:
    bool Read(const std::string &file);
    bool ReadString(const StringList &list);
    std::vector<CJsonLine>& GetLines() { return m_lines; }

private:
    std::vector<CJsonLine> m_lines;
};

#endif

#include <cstring>
#include "coder.h"
#include "log.h"

CCoder::CCoder(char* buff, uint size)
{
    m_begin  = buff;
    m_offset = buff;
    m_size   = size;
    m_error  = false;
}

CCoder::~CCoder()
{
}

// 简单类型编解码操作，不再调用<<或>>操作符函数，提高效率
#define ENCODE_SIMPLE(T, V)  *((T*)m_offset) = V; m_offset += sizeof(T)
#define DECODE_SIMPLE(T, V)  V = *((T*)m_offset); m_offset += sizeof(T)

#define CHECK_ERROR(SIZE)  \
    if(m_error){ return; } \
    if(m_offset + (SIZE) > m_begin + m_size){ m_error = true; return; }

// std::string
void CCoder::operator<<(const std::string &str)
{
    CHECK_ERROR(sizeof(uint));
    uint size = str.size();
    ENCODE_SIMPLE(uint, size);

    if(size > 0)
    {
        CHECK_ERROR(size);
        strncpy(m_offset, str.c_str(), size);
        m_offset += size;
    }
}
void CCoder::operator>>(std::string &str)
{
    CHECK_ERROR(sizeof(uint));
    uint size = 0;
    DECODE_SIMPLE(uint, size);

    if(size > 0)
    {
        CHECK_ERROR(size);
        str.assign(m_offset, size);
        m_offset += size;
    }
}

// S2(T - type, ST - sub type)
#define CODE_S2(T, ST)                   \
    void CCoder::operator <<(const T &s) \
    {                                    \
        CHECK_ERROR(sizeof(s));          \
        ENCODE_SIMPLE(ST, s.id);         \
        ENCODE_SIMPLE(ST, s.value);      \
    }                                    \
    void CCoder::operator >>(T &s)       \
    {                                    \
        CHECK_ERROR(sizeof(s));          \
        DECODE_SIMPLE(ST, s.id);         \
        DECODE_SIMPLE(ST, s.value);      \
    }
CODE_S2(S2UInt,  uint)
CODE_S2(S2ULong, ulong)

// S3(T - type, ST - sub type)
#define CODE_S3(T, ST)                   \
    void CCoder::operator <<(const T &s) \
    {                                    \
        CHECK_ERROR(sizeof(s));          \
        ENCODE_SIMPLE(ST, s.type);       \
        ENCODE_SIMPLE(ST, s.id);         \
        ENCODE_SIMPLE(ST, s.value);      \
    }                                    \
    void CCoder::operator >>(T &s)       \
    {                                    \
        CHECK_ERROR(sizeof(s));          \
        DECODE_SIMPLE(ST, s.type);       \
        DECODE_SIMPLE(ST, s.id);         \
        DECODE_SIMPLE(ST, s.value);      \
    }
CODE_S3(S3UInt,  uint)
CODE_S3(S3ULong, ulong)

// SBytes
void CCoder::operator<<(const SBytes &sb)
{
    CHECK_ERROR(sizeof(uint));
    ENCODE_SIMPLE(uint, sb.size);

    if(sb.size > 0)
    {
        CHECK_ERROR(sb.size);
        memcpy(m_offset, sb.bytes, sb.size);
        m_offset += sb.size;
    }
}
void CCoder::operator>>(SBytes &sb)
{
    CHECK_ERROR(sizeof(uint));
    DECODE_SIMPLE(uint, sb.size);

    if(sb.size > 0)
    {
        CHECK_ERROR(sb.size);
        memcpy(sb.bytes, m_offset, sb.size);
        m_offset += sb.size;
    }
}


// ----------------------------------------------------------------
// CODE_BASIC(T - type)
#define CODE_BASIC(T)                   \
    void CCoder::operator<<(const T &v) \
    {                                   \
        CHECK_ERROR(sizeof(T))          \
        ENCODE_SIMPLE(T, v);            \
    }                                   \
    void CCoder::operator>>(T &v)       \
    {                                   \
        CHECK_ERROR(sizeof(T));         \
        DECODE_SIMPLE(T, v);            \
    }


// CODE_SIMPLE_LIST(T - type, ST - sub type)
#define CODE_SIMPLE_LIST(T, ST)             \
    void CCoder::operator <<(const T &list) \
    {                                       \
        CHECK_ERROR(sizeof(uint));          \
        ENCODE_SIMPLE(uint, list.size());   \
        FORLIST(list, i)                    \
        {                                   \
            CHECK_ERROR(sizeof(ST));        \
            ENCODE_SIMPLE(ST, list[i]);     \
        }                                   \
    }                                       \
    void CCoder::operator >>(T &list)       \
    {                                       \
        list.clear();                       \
        CHECK_ERROR(sizeof(uint));          \
        uint size = 0;                      \
        DECODE_SIMPLE(uint, size);          \
        FOR(size, i)                        \
        {                                   \
            CHECK_ERROR(sizeof(ST));        \
            ST value;                       \
            DECODE_SIMPLE(ST, value);       \
            list.push_back(value);          \
        }                                   \
    }


// CODE_LIST(T - type, ST - sub type)
#define CODE_LIST(T, ST)                    \
    void CCoder::operator <<(const T &list) \
    {                                       \
        CHECK_ERROR(sizeof(uint));          \
        ENCODE_SIMPLE(uint, list.size());   \
        FORLIST(list, i)                    \
        {                                   \
            *this << list[i];               \
            if(m_error) { break; }          \
        }                                   \
    }                                       \
    void CCoder::operator >>(T &list)       \
    {                                       \
        list.clear();                       \
        CHECK_ERROR(sizeof(uint));          \
        uint size = 0;                      \
        DECODE_SIMPLE(uint, size);          \
        FOR(size, i)                        \
        {                                   \
            ST value;                       \
            *this >> value;                 \
            if(m_error) { break; }          \
            list.push_back(value);          \
        }                                   \
    }


// CODE_MAP(T - type, KT - key type, VT - value type)
#define CODE_MAP(T, KT, VT)                \
    void CCoder::operator <<(const T &map) \
    {                                      \
        CHECK_ERROR(sizeof(uint));         \
        ENCODE_SIMPLE(uint, map.size());   \
        FORMAP(map, iter)                  \
        {                                  \
            *this << iter->first;          \
            *this << iter->second;         \
            if(m_error) { break; }         \
        }                                  \
    }                                      \
    void CCoder::operator >>(T &map)       \
    {                                      \
        map.clear();                       \
        CHECK_ERROR(sizeof(uint));         \
        uint size = 0;                     \
        DECODE_SIMPLE(uint, size);         \
        FOR(size, i)                       \
        {                                  \
            KT key;                        \
            VT value;                      \
            *this >> key;                  \
            *this >> value;                \
            if(m_error) { break; }         \
            map[key] = value;              \
        }                                  \
    }


// ----------------------------------------------------------------
CODE_BASIC(bool)
CODE_BASIC(uchar)
CODE_BASIC(uint)
CODE_BASIC(ulong)

CODE_SIMPLE_LIST(BoolList,  bool)
CODE_SIMPLE_LIST(UCharList, uchar)
CODE_SIMPLE_LIST(UIntList,  uint)
CODE_SIMPLE_LIST(ULongList, ulong)
CODE_LIST(StringList,       std::string)
CODE_LIST(S2UIntList,       S2UInt)
CODE_LIST(S2ULongList,      S2ULong)
CODE_LIST(S3UIntList,       S3UInt)
CODE_LIST(S3ULongList,      S3ULong)

CODE_MAP(UIntBoolMap,      uint,        bool)
CODE_MAP(UIntUCharMap,     uint,        uchar)
CODE_MAP(UIntUIntMap,      uint,        uint)
CODE_MAP(UIntULongMap,     uint,        ulong)
CODE_MAP(UIntStringMap,    uint,        std::string)
CODE_MAP(UIntS2UIntMap,    uint,        S2UInt)
CODE_MAP(UIntS2ULongMap,   uint,        S2ULong)
CODE_MAP(UIntS3UIntMap,    uint,        S3UInt)
CODE_MAP(UIntS3ULongMap,   uint,        S3ULong)
CODE_MAP(StringBoolMap,    std::string, bool)
CODE_MAP(StringUCharMap,   std::string, uchar)
CODE_MAP(StringUIntMap,    std::string, uint)
CODE_MAP(StringULongMap,   std::string, ulong)
CODE_MAP(StringStringMap,  std::string, std::string)
CODE_MAP(StringS2UIntMap,  std::string, S2UInt)
CODE_MAP(StringS2ULongMap, std::string, S2ULong)
CODE_MAP(StringS3UIntMap,  std::string, S3UInt)
CODE_MAP(StringS3ULongMap, std::string, S3ULong)


#undef CHECK_ERROR
#undef ENCODE_SIMPLE
#undef DECODE_SIMPLE
#undef CODE_S2
#undef CODE_S3
#undef CODE_BASIC
#undef CODE_SIMPLE_LIST
#undef CODE_LIST
#undef CODE_MAP

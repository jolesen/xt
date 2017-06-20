#ifndef _COMMON_CORE_CODER_H_
#define _COMMON_CORE_CODER_H_

#include "core.h"

// CCoder
class CCoder
{
public:
    CCoder(char* buff, uint size);
    ~CCoder();

public:
    char* GetBuffer() { return m_begin; }
    uint  GetCodedSize() { return (m_offset - m_begin); }
    bool  HasError() { return m_error; }

    #define CODE(T)  void operator <<(const T &value); void operator >>(T &value)
    CODE(bool);
    CODE(uchar);
    CODE(uint);
    CODE(ulong);
    CODE(std::string);
    CODE(SBytes);
    CODE(S2UInt);
    CODE(S2ULong);
    CODE(S3UInt);
    CODE(S3ULong);
    CODE(BoolList);
    CODE(UCharList);
    CODE(UIntList);
    CODE(ULongList);
    CODE(StringList);
    CODE(S2UIntList);
    CODE(S2ULongList);
    CODE(S3UIntList);
    CODE(S3ULongList);
    CODE(UIntBoolMap);
    CODE(UIntUCharMap);
    CODE(UIntUIntMap);
    CODE(UIntULongMap);
    CODE(UIntStringMap);
    CODE(UIntS2UIntMap);
    CODE(UIntS2ULongMap);
    CODE(UIntS3UIntMap);
    CODE(UIntS3ULongMap);
    CODE(StringBoolMap);
    CODE(StringUCharMap);
    CODE(StringUIntMap);
    CODE(StringULongMap);
    CODE(StringStringMap);
    CODE(StringS2UIntMap);
    CODE(StringS2ULongMap);
    CODE(StringS3UIntMap);
    CODE(StringS3ULongMap);
    #undef CODE

private:
    char* m_begin;
    char* m_offset;
    uint  m_size;
    bool  m_error;
};

#endif

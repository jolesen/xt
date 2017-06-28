#ifndef _COMMON_CORE_BITS_H_
#define _COMMON_CORE_BITS_H_

#include "core.h"

class CBits
{
public:
    CBits();
    ~CBits();

    void Set(uint index, bool value);
    bool Get(uint index);

    const UIntList& GetValues() { return mValues; }
    void SetValues(const UIntList &values) { mValues = values; }

private:
    UIntList mValues;
};

#endif

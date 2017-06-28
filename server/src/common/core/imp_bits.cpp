#include "bits.h"

CBits::CBits()
{
}

CBits::~CBits()
{
}

void CBits::Set(uint index, bool value)
{
    uint arrIndex = index / UINT_BITS;
    uint bitIndex = index % UINT_BITS;

    if(arrIndex >= mValues.size()) // 扩容
    {
        FOR(arrIndex - mValues.size() + 1, i)
        {
            mValues.push_back(0);
        }
    }

    if(value)
    {
        mValues[arrIndex] |= (1 << bitIndex);
    }
    else
    {
        mValues[arrIndex] &= ~(1 << bitIndex);
    }
}

bool CBits::Get(uint index)
{
    uint arrIndex = index / UINT_BITS;
    if(arrIndex >= mValues.size())
    {
        return false;
    }

    uint bitIndex = index % UINT_BITS;
    return mValues[arrIndex] & (1 << bitIndex);
}

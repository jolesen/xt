#include "bits.h"

CBits::CBits()
{
}

CBits::~CBits()
{
}

void CBits::Set(uint index, bool value)
{
    uint arr_index = index / UINT_BITS;
    uint bit_index = index % UINT_BITS;

    if(arr_index >= m_values.size()) // 扩容
    {
        FOR(arr_index - m_values.size() + 1, i)
        {
            m_values.push_back(0);
        }
    }

    if(value)
    {
        m_values[arr_index] |= (1 << bit_index);
    }
    else
    {
        m_values[arr_index] &= ~(1 << bit_index);
    }
}

bool CBits::Get(uint index)
{
    uint arr_index = index / UINT_BITS;
    if(arr_index >= m_values.size())
    {
        return false;
    }

    uint bit_index = index % UINT_BITS;
    return m_values[arr_index] & (1 << bit_index);
}

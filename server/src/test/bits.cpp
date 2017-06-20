#include "common/core/bits.h"

void TestBits()
{
    uint size = 300;
    CBits bits;
    FOR(size, i)
    {
        bits.Set(i, i % 3 == 0);
        // 01001001001001001001001001001001
    }
    printf("\n");
    const UIntList &values = bits.GetValues();
    FORLIST(values, i)
    {
        printf("%u\t-> ", values[i]);
        for(int j = 31; j >= 0; --j)
        {
            printf("%u", bits.Get(i * 32 + j));
        }
        printf("\n");
    }
}



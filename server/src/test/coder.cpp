#include <cstring>
#include "common/core/coder.h"
#include "common/core/util.h"

void _TestCoderUIntList()
{
    printf("---- test coder UIntList ---- \n");
    // encode
    UIntList list_encode;
    FOR(10, i)
    {
        uint rand = CUtil::Rand(100);
        list_encode.push_back(rand);
        printf("%u ", list_encode[i]);
    }
    char buff_encode[128] = { 0 };
    CCoder encoder(buff_encode, sizeof(buff_encode));
    encoder << list_encode;
    printf("\tencoded size:%u\n", encoder.GetCodedSize());

    // decode
    char buff_decode[128] = { 0 };
    memcpy(buff_decode, buff_encode, sizeof(buff_encode));
    CCoder decoder(buff_decode, sizeof(buff_decode));
    UIntList list_decode;
    decoder >> list_decode;
    FORLIST(list_decode, i)
    {
        printf("%u ", list_decode[i]);
    }
    printf("\tdecoded size:%u\n", decoder.GetCodedSize());
}

void _TestCoderStringList()
{
    printf("---- test coder StringList ---- \n");
    // encode
    StringList list_encode;
    FOR(10, i)
    {
        uint rand = CUtil::Rand(100);
        list_encode.push_back(CUtil::ToString(rand));
        printf("%s ", list_encode[i].c_str());
    }
    char buff_encode[128] = { 0 };
    CCoder encoder(buff_encode, sizeof(buff_encode));
    encoder << list_encode;
    printf("\tencoded size:%u\n", encoder.GetCodedSize());

    // decode
    char buff_decode[128] = { 0 };
    memcpy(buff_decode, buff_encode, sizeof(buff_encode));
    CCoder decoder(buff_decode, sizeof(buff_decode));
    StringList list_decode;
    decoder >> list_decode;
    FORLIST(list_decode, i)
    {
        printf("%s ", list_decode[i].c_str());
    }
    printf("\tdecoded size:%u\n", decoder.GetCodedSize());
}





void TestCoder()
{
    _TestCoderUIntList();
    _TestCoderStringList();
}

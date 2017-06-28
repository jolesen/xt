#include <cstring>
#include "common/core/coder.h"
#include "common/core/util.h"

void _TestCoderUIntList()
{
    printf("---- test coder UIntList ---- \n");
    // encode
    UIntList listEncode;
    FOR(10, i)
    {
        uint rand = CUtil::Rand(100);
        listEncode.push_back(rand);
        printf("%u ", listEncode[i]);
    }
    char buffEncode[128] = { 0 };
    CCoder encoder(buffEncode, sizeof(buffEncode));
    encoder << listEncode;
    printf("\tencoded size:%u\n", encoder.GetCodedSize());

    // decode
    char buffDecode[128] = { 0 };
    memcpy(buffDecode, buffEncode, sizeof(buffEncode));
    CCoder decoder(buffDecode, sizeof(buffDecode));
    UIntList listDecode;
    decoder >> listDecode;
    FORLIST(listDecode, i)
    {
        printf("%u ", listDecode[i]);
    }
    printf("\tdecoded size:%u\n", decoder.GetCodedSize());
}

void _TestCoderStringList()
{
    printf("---- test coder StringList ---- \n");
    // encode
    StringList listEncode;
    FOR(10, i)
    {
        uint rand = CUtil::Rand(100);
        listEncode.push_back(CUtil::ToString(rand));
        printf("%s ", listEncode[i].c_str());
    }
    char buffEncode[128] = { 0 };
    CCoder encoder(buffEncode, sizeof(buffEncode));
    encoder << listEncode;
    printf("\tencoded size:%u\n", encoder.GetCodedSize());

    // decode
    char buffDecode[128] = { 0 };
    memcpy(buffDecode, buffEncode, sizeof(buffEncode));
    CCoder decoder(buffDecode, sizeof(buffDecode));
    StringList listDecode;
    decoder >> listDecode;
    FORLIST(listDecode, i)
    {
        printf("%s ", listDecode[i].c_str());
    }
    printf("\tdecoded size:%u\n", decoder.GetCodedSize());
}





void TestCoder()
{
    _TestCoderUIntList();
    _TestCoderStringList();
}

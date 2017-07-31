#include <stdio.h>

int main(int argc, char* argv[])
{
    short s = 0x1234;
    char a0 = ((char*)(&s))[0];
    char a1 = ((char*)(&s))[1];

    printf("value: 0x%x, a0: 0x%x, a1: 0x%x\n", s, a0, a1);
    if(a0 == 0x34)
    {
        printf("little endian\n");
    }
    else
    {
        printf("big endian\n");
    }

    return 0;
}

#include <stdio.h>

void Reverse(char *str, int len)
{
    int left  = 0;
    int right = len - 1;
    while(left < right)
    {
        char c     = str[left];
        str[left]  = str[right];
        str[right] = c;

        ++left;
        --right;
    }
}

int main(int argc, char* argv[])
{
    char str[] = "my name is xx";
    printf("reverse of [%s] ", str);
    Reverse(str, sizeof(str) - 1);
    printf("is [%s]\n", str);

    return 0;
}

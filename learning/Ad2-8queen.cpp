#include <stdio.h>
#include <stdlib.h>

#define MAX 8
int gQueen[MAX];
int gSum = 0;

void Show()
{
    gSum++;
    for(int i = 0; i < MAX; i++)
    {
        printf("(%d,%d) ", i, gQueen[i]);
    }
    printf(" - %d\n", gSum);
    fflush(stdout);
}   

// 检查当前列能否放置皇后
bool Check(int n)
{
    for(int i = 0; i < n; i++) // 检查横排和对角线上是否可以放置皇后
    {
        if((gQueen[i] == gQueen[n]) || (abs(gQueen[i] - gQueen[n]) == (n - i)))
        {
            return true;
        }
    }
    return false;
}

// 回溯尝试皇后位置,n为横坐标
void Put(int n)
{
    for(int i = 0; i < MAX; i++)
    {
        gQueen[n] = i; // 将皇后摆到当前循环到的位置
        if(!Check(n))
        {
            if(n >= MAX - 1)
            {
                Show(); // 如果全部摆好，则输出所有皇后的坐标
            }
            else
            {
                Put(n + 1); // 否则继续摆放下一个皇后
            }
        }
    }
}

int main(int argc, char* argv[])
{
    gSum = 0;
    Put(0); // 从横坐标为0开始依次尝试
    printf("%d\n", gSum);
    return 0;
}

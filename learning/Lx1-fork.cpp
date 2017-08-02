#include <unistd.h>
#include <stdio.h>

void basic()
{
    int count = 0;
    pid_t pidParent = getpid();
    printf("I will doing fork, pid:%d\n", pidParent);

    pid_t pidFork = fork();
    if(pidFork < 0)
    {
        printf("fork error\n");
    }
    else if(pidFork == 0)
    {
        printf("I am child[pid:%d] of parent[pid:%d]\n", getpid(), pidParent);
        count++;
    }
    else
    {
        printf("I am parent[pid:%d], child:%d\n", pidParent, pidFork);
        count++;
    }
    printf("count: %d\n", count);
}

int main(int argc, char* argv[])
{
    basic();
    return 0;
}

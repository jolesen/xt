#include <stdio.h>

long double gMoveCount = 0;

void Move(int n, char from, char to)
{
    //printf("move %d, %c -> %c\n", n, from, to);
    ++gMoveCount;
}

void Hanoi(int n, char from, char assist, char to)
{
    if(n <= 1)
    {
        Move(n, from, to);
        return;
    }

    Hanoi(n - 1, from, to, assist);
    Move(n, from, to);
    Hanoi(n - 1, assist, from, to);
}

int main(int argc, char* argv[])
{
    gMoveCount = 0;
    Hanoi(10, 'A', 'B', 'C');
    printf("move count: %Lf\n", gMoveCount);

    return 0;
}

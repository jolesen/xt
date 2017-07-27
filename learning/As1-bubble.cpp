#include<stdio.h>

void BubbleSort(int a[], int n)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = n - 1; j > i; j--)
        {
            if(a[j] < a[j - 1])
            {
                int tmp = a[j];
                a[j] = a[j - 1];
                a[j - 1] = tmp;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int array[] = {1, 2, 34, 56, 3, 9, 81, 78, 45, 2, 4, 5, 10, 6};
    int len = sizeof(array) / sizeof(int);

    printf("before quick sort: ");
    for(int i = 0; i < len; i++)
    {
        printf("%d", array[i]);
        printf("%s", (i == len - 1) ? "\n" : ", ");
    }

    BubbleSort(array, len);

    printf("after  quick sort: ");
    for(int i = 0; i < len; i++)
    {
        printf("%d", array[i]);
        printf("%s", (i == len - 1) ? "\n" : ", ");
    }

    return 0;
}

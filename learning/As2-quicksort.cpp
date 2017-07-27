#include<stdio.h>

void QuickSort(int a[], int l, int r)
{
    if(l < r)
    {
        int i = l;
        int j = r;
        int v = a[i];

        while(i < j)
        {
            // find smaller from right to left
            while((i < j) && a[j] > v)
            {
                --j;
            }
            if(i < j)
            {
                a[i++] = a[j];
            }

            // find bigger from left to right
            while((i < j) && (a[i] < v))
            {
                ++i;
            }
            if(i < j)
            {
                a[j--] = a[i];
            }
        }
        a[i] = v;
        QuickSort(a, l, i - 1);
        QuickSort(a, i + 1, r);
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

    QuickSort(array, 0, len - 1);

    printf("after  quick sort: ");
    for(int i = 0; i < len; i++)
    {
        printf("%d", array[i]);
        printf("%s", (i == len - 1) ? "\n" : ", ");
    }

    return 0;
}

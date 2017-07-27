#include <stdio.h>
#include <stdint.h>

uint64_t fibonacci(unsigned int n)
{
    if(n == 0)
    {
        return 0;
    }
    if(n == 1)
    {
        return 1;
    }

    return fibonacci(n - 1) + fibonacci(n - 2);
}

uint64_t fibonacci_no_recursive(unsigned int n)
{
    if(n == 0)
    {
        return 0;
    }
    if(n == 1)
    {
        return 1;
    }

    uint64_t fn  = 0;
    uint64_t fn1 = 1;
    uint64_t fn2 = 0;
    for(unsigned int i = 2; i <= n; i++)
    {
        fn  = fn1 + fn2;
        fn1 = fn;
        fn2 = fn1;
    }
    return fn;
}

// save temp value
uint64_t Fibonacci(unsigned int n)
{
    static uint64_t fib[256] = {0, 1};
    if(n == 0) return 0;
    if( fib[n] != 0 ) return fib[n];
    fib[n] = Fibonacci(n - 1) + Fibonacci(n - 2);
    return fib[n];
}

int main(int argc, char* argv[])
{
    unsigned int n = 30;
    uint64_t value = fibonacci(n);
    printf("Fibonacci(%d) is %lld\n", n, value);

    //n = 30;
    value = fibonacci_no_recursive(n);
    printf("Fibonacci(%d) is %lld\n", n, value);

    return 0;
}

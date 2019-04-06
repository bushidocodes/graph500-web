#include <stdio.h>

// Calculate Fibonacci numbers shared function
int fibonacci(int iterations)
{
    int val = 1;
    int last = 0;

    if (iterations == 0)
    {
        return 0;
    }
    for (int i = 1; i < iterations; i++)
    {
        int seq;

        seq = val + last;
        last = val;
        val = seq;
    }
    return val;
}

// Foreground thread and main entry point
int main(int argc, char *argv[])
{
    int desiredSolutions[] = {19, 20, 21, 22, 23};
    for (int i = 0; i < sizeof(desiredSolutions) / sizeof(int); i++)
    {
        printf("Forground: Fib(%d) is %d\n", desiredSolutions[i], fibonacci(desiredSolutions[i]));
    }
    return 0;
}
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
#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 4

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
// Start function for the background thread
void *bg_func(void *arg)
{
    int *iter = (void *)arg;

    *iter = fibonacci(*iter);
    return arg;
}
// Foreground thread and main entry point
int main(int argc, char *argv[])
{
    int fg_val = 19;
    int fg_result;
    int bg_vals[] = {20, 21, 22, 23};
    int bg_results[4];
    for (int i = 0; i < sizeof(bg_vals) / sizeof(int); i++)
    {
        bg_results[i] = bg_vals[i];
    }
    pthread_t bg_thread[NUM_THREADS];

    for (int i = 1; i <= NUM_THREADS; i++)
    {
        if (pthread_create(&bg_thread[i - 1], NULL, bg_func, &(bg_results[i - 1])))
        {
            fprintf(stderr, "Failed to allocate thread %d", i);
            return 1;
        }
    }
    // Create the background thread
    // Calculate on the foreground thread
    fg_result = fibonacci(fg_val);
    // Wait for background thread to finish

    for (int i = 1; i <= NUM_THREADS; i++)
    {
        if (pthread_join(bg_thread[i - 1], NULL))
        {
            fprintf(stderr, "Failed to join thread %d", i);
            return 2;
        }
    }
    // Show the result from background and foreground threads
    printf("Forground: Fib(%d) is %d\n", fg_val, fg_result);
    for (int i = 1; i <= NUM_THREADS; i++)
    {
        printf("Background Thread %d: Fib(%d) is %d\n", i, bg_vals[i - 1], bg_results[i - 1]);
    }

    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "constants.h"

/* Array Based Queue */

typedef struct queue
{
    int32_t data[MAXV]; /* Array that stores the queue */
    int32_t start;      /* idx of the start of the queue */
    int32_t end;        /* ids of the end of the queue (exclusive) */
} queue;

void enqueue(int32_t x, queue *q)
{
    if (q->end >= MAXV)
    {
        printf("Error: Queue is full");
        /* TODO: Implement queue cleanup...*/
        return;
    }
    q->data[q->end - 1] = x;
    q->end++; // Not thread-safe... Not atomic
}

int32_t dequeue(queue *q)
{
    if (q->start == q->end - 1)
    {
        printf("Error: Queue is empty\n");
        // exit(99);
    }
    int32_t result = q->data[q->start];
    q->start++;
    return result;
}

int32_t len(queue *q)
{
    return (q->end - 1) - q->start;
}

void reset(queue *q, bool should_zero_data)
{
    if (should_zero_data)
    {
        for (int32_t i = 0; i <= q->end; i++)
        {
            q->data[i] = 0;
        }
    }
    q->start = 0;
    q->end = 1;
}

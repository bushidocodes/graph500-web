#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"
#include "graph.c"
#include "queue.c"

// void initialize_search(graph *g);
void bfs(graph *g, int32_t start, bool is_discovered[MAXV + 1], int32_t has_parent[MAXV + 1]);
void print_parents(graph *g, int32_t has_parent[MAXV + 1]);

void bfs(graph *g, int32_t start, bool is_discovered[MAXV + 1], int32_t has_parent[MAXV + 1])
{
    queue *q;
    int32_t vertex;
    int32_t neighbor_count = 0;
    int32_t adjacentVertex;
    edge *edgeLinkedList;

    // Initialize queue
    q = malloc(sizeof(queue));
    reset(q, false);

    // Add start to queue
    enqueue(start, q);

    // And set to discovered
    is_discovered[start] = true;
    // And set self as parent
    has_parent[start] = start;

    while (len(q) > 0)
    {
        vertex = dequeue(q);
        edgeLinkedList = g->edges[vertex];
        while (edgeLinkedList != NULL)
        {
            adjacentVertex = edgeLinkedList->destination;
            if (is_discovered[adjacentVertex] == false)
            {
                enqueue(adjacentVertex, q);
                is_discovered[adjacentVertex] = true;
                has_parent[adjacentVertex] = vertex;
            }
            edgeLinkedList = edgeLinkedList->next;
        }
    }
}

void print_parents(graph *g, int32_t has_parent[MAXV + 1])
{
    for (int32_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%d has parent %d\n", i, has_parent[i]);
    }
}

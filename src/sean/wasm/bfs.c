#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"
#include "graph.c"
#include "queue.c"

void bfs(graph *g, int32_t start, bool is_discovered[MAXV + 1], int32_t has_parent[MAXV + 1]);
void print_parents(graph *g, int32_t has_parent[MAXV + 1]);

void bfs(graph *g, int32_t start, bool is_discovered[MAXV + 1], int32_t has_parent[MAXV + 1])
{
    if (g->is_CSR)
    {
        uint8_t *level = malloc((g->number_vertices + 1) * sizeof(uint8_t));
        memset(level, UINT8_MAX, (g->number_vertices + 1) * sizeof(uint8_t));
        uint8_t current_level = 0;
        int32_t neighbor_count = 0;
        bool should_advance = true;

        // And set self as parent
        has_parent[start] = start;
        level[start] = 0;
        while (should_advance)
        {
            should_advance = false;
            for (int32_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
            {
                if (level[current_vertex] == current_level)
                {
                    neighbor_count = getDegree(g, current_vertex);
                    // TODO: I inexplicably need to pad this number... Why?
                    int32_t *neighbors = malloc((getDegree(g, current_vertex) + 200) * sizeof(int32_t));
                    getNeighbors(g, current_vertex, neighbors);

                    for (int32_t i = 0; i < neighbor_count; i++)
                    {
                        if (level[neighbors[i]] == UINT8_MAX)
                        {
                            should_advance = true;
                            level[neighbors[i]] = current_level + 1;
                            has_parent[neighbors[i]] = current_vertex;
                        }
                    }
                    free(neighbors);
                }
            }
            current_level++;
        }
        free(level);
    }
    else
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
}

void print_parents(graph *g, int32_t has_parent[MAXV + 1])
{
    for (int32_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%d has parent %d\n", i, has_parent[i]);
    }
}

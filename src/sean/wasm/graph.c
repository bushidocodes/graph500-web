#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "constants.h"

// This edge struct acts as a linked list node
typedef struct edge
{
    int32_t destination; /* destination vertex of edge a.k.a an adjascent vertex */
    struct edge *next;   /* next edge in list... I have to use the struct keyboard because this is recursive */
} edge;

typedef struct graph
{
    edge *edges[MAXV + 1];    /* adjacency info */
    int32_t degree[MAXV + 1]; /* outdegree of each vertex */
    int32_t number_vertices;  /* number of vertices in graph */
    int32_t number_edges;     /* number of edges in graph */
    bool is_directed;         /* Is the graph directed */
} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, int32_t x, int32_t y, bool directed);
void print_graph(graph *g);

int32_t getDegree(graph *g, int32_t vertex);
// int getNeighbors(graph *g, int source, int results[]);

int32_t getDegree(graph *g, int32_t vertex)
{
    return g->degree[vertex];
}

// This initialization takes a while because the matrix is huge
void initialize_graph(graph *g, bool directed)
{
    // Using memset to zero out an array
    for (int32_t i = 0; i <= MAXV; i++)
    {
        for (int32_t j = 0; j <= MAXV; j++)
        {
            g->edges[i] = NULL;
            g->degree[i] = 0;
        }
    }
    memset(g->degree, 0, (MAXV + 1) * sizeof(int32_t));
    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
    printf("Graph Initialization complete\n");
}

void read_graph(graph *g, bool is_directed)
{
    int32_t edge_count;          /* number of edges */
    int32_t source, destination; /* vertices in edge (source, destination) */

    scanf("%d %d", &(g->number_vertices), &edge_count);

    for (int32_t i = 1; i <= edge_count; i++)
    {
        scanf("%d %d", &source, &destination);
        insert_edge(g, source, destination, is_directed);
    }
}

// Inserts an edge from source to destination in the adjascency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once.
void insert_edge(graph *g, int32_t source, int32_t destination, bool is_directed)
{
    printf("Insert %d %d\n", source, destination);

    edge *new_edge;                  /* temporary pointer */
    new_edge = malloc(sizeof(edge)); /* allocate edge storage */
    if (new_edge != NULL)
    {
        printf("Successfully alloced.\n");
    }

    new_edge->destination = destination;
    new_edge->next = g->edges[source];

    g->edges[source] = new_edge; //68001 is max
    g->degree[source]++;

    if (is_directed == false)
    {
        // We set direted to true in this call so we don't infinitely loop
        insert_edge(g, destination, source, true);
    }
    else
    {
        // By incrementing in this else block, we only increment one for undirected graphs
        g->number_edges++;
    }
}

// Takes a pointer to an array, returns the length of the array
// int getNeighbors(graph *g, int source, int results[])
// {
//     int results_length = 0;
//     for (int destination = 1; destination <= g->number_vertices; destination++)
//     {
//         if (g->edges[source][destination])
//         {
//             results[results_length] = destination;
//             results_length++;
//         }
//     }
//     return results_length;
// }

void print_graph(graph *g)
{
    edge *p; /* temporary pointer */
    for (int32_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%d: ", i);
        p = g->edges[i];
        while (p != NULL)
        {
            printf(" %d", p->destination);
            p = p->next;
        }
        printf("\n");
    }
}
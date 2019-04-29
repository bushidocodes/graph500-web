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
    int32_t *IA;              /* Cumulative count of non zero values excluding the row at this index */
    int32_t *JA;              /* Actual Column Indices of non zero values. Use IA first... */
    bool is_CSR;
} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, int32_t x, int32_t y, bool directed);
void print_graph(graph *g);
void build_csr(graph *g);
int32_t getDegree(graph *g, int32_t vertex);
void getNeighbors(graph *g, int32_t source, int32_t results[]);

int32_t getDegree(graph *g, int32_t vertex)
{
    if (g->is_CSR)
        return g->IA[vertex + 1] - g->IA[vertex];
    else
        return g->degree[vertex];
}

// This initialization takes a while because the matrix is huge
void initialize_graph(graph *g, bool directed)
{
    for (int32_t i = 0; i <= MAXV; i++)
    {
        g->edges[i] = NULL;
        g->degree[i] = 0;
    }

    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
    g->is_CSR = false;
    printf("Graph Initialization complete\n");
}

// Inserts an edge from source to destination in the adjascency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once.
void insert_edge(graph *g, int32_t source, int32_t destination, bool is_directed)
{
    int32_t max = source > destination ? source : destination;

    if (max > g->number_vertices)
        g->number_vertices = max;

    edge *new_edge;                          /* temporary pointer */
    new_edge = (edge *)malloc(sizeof(edge)); /* allocate edge storage */
    if (new_edge == NULL)
    {
        printf("Malloc failed!");
    }

    new_edge->destination = destination;
    if (g->edges[source] != NULL)
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

void build_csr(graph *g)
{
    g->IA = malloc((g->number_edges + 2) * sizeof(int32_t)); /* I might be off by one here */

    // Mark the 0 positions as Evil to force a crash to ensure I'm indexing starting at 1 to be consistent
    g->IA[0] = 666;

    g->IA[1] = 0;
    // I'm just multiplying by 2 here because the number_edges is only incremented one for the opposing edges on a non-DiGraph. I should really only store non-directed edges once.
    g->JA = malloc((g->number_edges * 2) * sizeof(int32_t)); // "Column Indices"
    int32_t edge_idx = 0;
    for (int32_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
    {
        for (edge *currentEdge = g->edges[current_vertex]; currentEdge->next != NULL; currentEdge = currentEdge->next)
        {
            g->JA[edge_idx] = currentEdge->destination;
            edge_idx++;
        }
        g->IA[current_vertex + 1] = edge_idx;
    }
    g->is_CSR = true;
}

// Takes a pointer to an array, returns the length of the array
void getNeighbors(graph *g, int32_t source, int32_t results[])
{
    if (g->is_CSR)
    {
        for (int32_t idx = 0; idx + g->IA[source] < g->IA[source + 1]; idx++)
        {
            results[idx] = g->JA[idx + g->IA[source]];
        }
    }
}

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